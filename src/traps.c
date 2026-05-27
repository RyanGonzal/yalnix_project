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
    trap_vector[TRAP_MEMORY] = trap_memory;
    WriteRegister(REG_VECTOR_BASE, (unsigned int)trap_vector);
}

void trap_kernel(UserContext *uctxt)
{
    pcb_t *entering_process;

    TracePrintf(0, "TRAP_KERNEL syscall code: 0x%x\n", uctxt->code);

    entering_process = current_process;

    if (entering_process != NULL) {
        entering_process->user_context = *uctxt;
    }

    syscall_handle(uctxt);

    if (current_process == entering_process && current_process != NULL) {
        current_process->kernel_context_valid = 1;
    }
}
void trap_clock(UserContext *uctxt)
{
    TracePrintf(0, "TRAP_CLOCK\n");

    if (current_process != NULL) {
        current_process->user_context = *uctxt;
    }

    if (init_process != NULL && init_process->state == PROC_BLOCKED) {
        init_process->delay_ticks--;

        if (init_process->delay_ticks <= 0) {
            scheduler_add(init_process);
        }
    }

    scheduler_run_next(uctxt);
}
    




// invalid memory requests or page table faults 
void trap_memory(UserContext *uctxt)
{
    // check faulting address
    // if valid stack:
    //     allocate and map new page
    // else:
    //     kill current process 
    TracePrintf(0, "TRAP_MEMORY addr=%p pid=%d\n",
                uctxt->addr, current_process->pid);

    process_exit_current(ERROR);

    if (current_process != NULL) {
        current_process = NULL;
    }

    scheduler_run_next(uctxt);
}

//for all missing traps 
void trap_unhandled(UserContext *uctxt)
{
    // print/debug unexpected trap
    // for now kill process?
    //might not need this bc ofd syscall_handle
    TracePrintf(0, "UNHANDLED TRAP vector=%d code=0x%x addr=%p\n",
            uctxt->vector, uctxt->code, uctxt->addr);

    (void)uctxt;
}
