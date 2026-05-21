// Ryan Gonzalez, Sam Hirsh traps.c
#include <ykernel.h>
#include "traps.h"
#include "process.h"
#include "syscalls.h"

// trap vector table used by hardware
static void (*trap_vector[TRAP_VECTOR_SIZE])(UserContext *);

//sets up trap table
void traps_init(void)
{
    // set all traps to unhandled
    for (int i = 0; i < TRAP_VECTOR_SIZE; i++) {
        trap_vector[i] = trap_unhandled;
    }
    
    trap_vector[TRAP_CLOCK] = trap_clock;
    trap_vector[TRAP_KERNEL] = trap_kernel;
    WriteRegister(REG_VECTOR_BASE, (unsigned int)trap_vector);
}


// handles sys_calls from user mode for kernel 
void trap_kernel(UserContext *uctxt)
{
    // save UserContext into current_process PCB
    // use uctxt  to .get syscall number
    // call syscall handler later
    // restore selected process UserContext 
    // return 
     TracePrintf(0, "TRAP_KERNEL syscall code: 0x%x\n", uctxt->code);
    (void)uctxt;
}

// timer interrupts 
void trap_clock(UserContext *uctxt)
{
    // save current UserContext
    // update sleep queues
    // if time met switch to other process
    // restore selected process UserContext
    // return
    TracePrintf(0, "TRAP_CLOCK\n");

    (void)uctxt;
}

// invalid memory requests or page table faults 
void trap_memory(UserContext *uctxt)
{
    // check faulting address
    // if valid stack:
    //     allocate and map new page
    // else:
    //     kill current process 

    (void)uctxt;
}

//for all missing traps 
void trap_unhandled(UserContext *uctxt)
{
    // print/debug unexpected trap
    // for now kill process?
    TracePrintf(0, "UNHANDLED TRAP vector=%d code=0x%x addr=%p\n",
            uctxt->vector, uctxt->code, uctxt->addr);

    (void)uctxt;
}