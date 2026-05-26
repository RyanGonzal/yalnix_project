// Ryan Gonzalez, Sam Hirsh kernel.c
#include <ykernel.h>
#include "memory.h"
#include "process.h"
#include "traps.h"
#include "syscalls.h"

void DoIdle(void)
{
    while(1){
        TracePrintf(1, "DoIdle\n");
        Pause();
    }
}
// main entry point 
void KernelStart(char *cmd_args[],unsigned int pmem_size, UserContext *uctxt)
{
    pcb_t *init;
    char *init_name;
    TracePrintf(0, "Kernel Start \n");
    (void)cmd_args;
    // initialize physical memory
    memory_init(pmem_size);
    memory_init_region0();
    memory_init_idle_region1();
    // initialize trap table
    traps_init();
    // create idle process

    uctxt->pc = DoIdle;
    uctxt->sp = memory_get_idle_stack_top();
    process_init();
    process_create_idle(uctxt);
    
    memory_enable_vm();
    //create PCB and empty region1
    init = process_create_init(uctxt);

    WriteRegister(REG_PTBR1, (unsigned int)init->region1_pt);
    WriteRegister(REG_TLB_FLUSH, TLB_FLUSH_1);

    if (cmd_args[0] != NULL) {
        init_name = cmd_args[0];
    } else {
        init_name = "init";
    }
    //loads the program
    if (LoadProgram(init_name, cmd_args, init) != SUCCESS) {
        helper_abort("KernelStart: LoadProgram failed");
    }
    //flush then leave kernel in idle
    WriteRegister(REG_PTBR1, (unsigned int)process_get_idle()->region1_pt);
    WriteRegister(REG_TLB_FLUSH, TLB_FLUSH_1);

    current_process = process_get_idle();

    TracePrintf(0, "Leaving KernelSart\n");
}

KernelContext *KCSwitch(KernelContext *KCin, void *current_pcb, void *next_pcb) {
    pcb_t *current = (pcb_t *)current_pcb;
    pcb_t *next = (pcb_t *)next_pcb;
    // Check that both pcbs are not null
    if (current == NULL) {
        helper_abort("KCSwitch: current_pcb is NULL");
    }
    if (next == NULL) {
        helper_abort("KCSwitch: next_pcb is NULL");
    }
    // Save current process kernel context
    current->kernel_context = *KCin;
    // loop through kernel stack and save each page in region 0 into current_pcbs stack frames
    // saving whatever is actually in memory into current pcb
    memory_save_current_kstack(current);
    // TODO: take next_pcb's stack frames into current_pcb's stack frames
    memory_restore_kstack(next);
    // TLB flush
    WriteRegister(REG_TLB_FLUSH, TLB_FLUSH_KSTACK);
    // set current process to next pcb
    current_process = next;
    // return next pcbs kernel context
    return &next->kernel_context;
}

KernelContext *KCCopy(KernelContext *KCin, void *new_pcb, void *not_used) {
    pcb_t *new = (pcb_t *)new_pcb;

    (void)not_used;

    if (new == NULL) {
        helper_abort("KCCopy: new_pcb is NULL");
    }

    new->kernel_context = *KCin;

    for (int i = 0; i < KSTACK_PAGES; i++) {
        int vpn = (KERNEL_STACK_BASE >> PAGESHIFT) + i;

        memory_copy_kstack_page(vpn, new->kstack_pfn[i]);
    }

    return &new->kernel_context;
}