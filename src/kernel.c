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

KernelContext KCSwitch(KernelContext *KCin, void *current_pcb, void *next_pcb) {
    // Check that both pcbs are not null
    if (current_pcb == NULL) {
        return NULL;
    }

    if (next_pcb == NULL) {
        return NULL;
    }

    // Save current process kernel context
    memcpy(current_pcb.kernel_context, KCin, sizeof(current_pcb.kernel_context))

    // loop through kernel stack and save each page in region 0 into current_pcbs stack frames
        // saving whatever is actually in memory into current pcb 
    for (int i = KERNEL_STACK_BASE >> PAGESHIFT; i < KERNEL_STACK_LIMIT >> PAGESHIFT; i++){
        // TODO: Here.
        // I think the pcb needs to have pointer to kernel stack?
        // This stack is set in process as discussed with TA (idle and init)
        current_pcb->kernel_stack_frames[i] = region0[i]
    }



    // TODO: take next_pcb's stack frames into current_pcb's stack frames
    
    // TLB flush
    WriteRegister(REG_TLB_FLUSH, TLB_FLUSH_0);
   
    // set current process to next pcb
    current_process = next_pcb;

    // return next pcbs kernel context
    return next_pcb->kernel_context
}

KernelContext KCCopy(KernelContext *KCin, void *new_pcb, void *) {
    // check if new pcb is not null
    if (new_pcb == NULL) {
        return NULL;
    }

    // copy kcin into new pcbs kernel context
    new_pcb->kernel_context = KCin;

    // copy new_pcbs kernel stack frame into pages region 0
    for (int i = KERNEL_STACK_BASE >> PAGESHIFT; i < KERNEL_STACK_LIMIT >> PAGESHIFT; i++){
        // TODO: Here
        // See above about pcb needs to have pointer to kernel stack?
    }

    WriteRegister(REG_TLB_FLUSH, TLB_FLUSH_0);
}