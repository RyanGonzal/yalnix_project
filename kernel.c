// Ryan Gonzalez, Sam Hirsh kernel.c
#include <ykernel.h>
#include "memory.h"
#include "process.h"
#include "traps.h"
#include "syscalls.h"

// main entry point 
void KernelStart(char *cmd_args[],unsigned int pmem_size,UserContext *uctxt)
{
    // initialize physical memory
    memory_init(pmem_size);
    // initialize process structures and queues
    process_init();
    // initialize trap table
    traps_init();
    // create idle process
    process_create_idle(uctxt);
    // TODO initiaul VM and prolly region 0 vs region 1

    (void)cmd_args;
}