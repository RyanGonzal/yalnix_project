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
void KernelStart(char *cmd_args[],unsigned int pmem_size,UserContext *uctxt)
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