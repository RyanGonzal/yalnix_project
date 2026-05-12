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
    process_create_idle(uctxt);
    
    memory_enable_vm();
    TracePrintf(0, "Leaving KernelSart\n");
}