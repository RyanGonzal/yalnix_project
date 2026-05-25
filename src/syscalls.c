// Ryan Gonzalez, Sam Hirsh syscalls.c

#include <ykernel.h>
#include "syscalls.h"
#include "process.h"
#include "memory.h"

// sends each syscall to the correct kernel handle
void syscall_dispatch(UserContext *uctxt)
{
    // check syscall number in uctxt
    // switch on syscall number
    // call correct Kernel syscall function
    // store return value in user context register

    (void)uctxt;
}

// creates child process
int KernelFork(UserContext *uctxt)
{
    
    // create child PCB
    // copy parent address space
    // child returns 0
    // parent returns child pid

    (void)uctxt;
    return ERROR;
}

// replaces current prog w new prog
int KernelExec(UserContext *uctxt)
{
    // get program name and arguments from user memory
    // call LoadProgram 
    // replace current Region 1 address space

    (void)uctxt;
    return ERROR;
}

// exit current process
int KernelExit(UserContext *uctxt)
{
    // get exit status
    // mark current process as zombie
    // schedule another process
    int status = uctxt->regs[0];

    process_exit_current(status);

    scheduler_run_next(uctxt);

    return 0;

    
}

// waits for child process to exit
int KernelWait(UserContext *uctxt)
{
    // while waiting for zombie child
    // get child pid and exit status
     int status;
    pcb_t *child;

    child = process_wait_for_child(&status);

    if (child == NULL) {
        return ERROR;
    }
    return child->pid;

}

// get current pid
// FOr CP 4
int KernelGetPid(UserContext *uctxt)
{
    // return current_process pid

    (void)uctxt;
    if (current_process == NULL) {
        return ERROR;
    }

    return current_process->pid;
}

// changes heap break 
int KernelBrk(UserContext *uctxt)
{
    // if break address is validated
    // grow or shrink, then update page table
    (void)uctxt;
    return 0;
}

// delays kernel by blocking clock
int KernelDelay(UserContext *uctxt)
{
    // get delay ticks
    // block current process
    // wake after delay
    int ticks = uctxt->regs[0];

    if (ticks <= 0) {
        return ERROR;
    }

    current_process->delay_ticks = ticks;

    scheduler_block_current();
    scheduler_run_next(uctxt);

    return 0;

}

// read from terminal via TTY
int KernelTtyRead(UserContext *uctxt)
{
    // while data 
    // validate user data
    // copy input to bugger


    (void)uctxt;
    return ERROR;
}

// write to terminal via TTY
int KernelTtyWrite(UserContext *uctxt)
{
    // if user buffer == valid
    // copt bytes to kernel buffer, call TTY transmit
    // delay until it is done 

    (void)uctxt;
    return ERROR;
}
void syscall_handle(UserContext *uctxt)
{
    switch (uctxt->code) {
        case YALNIX_GETPID:
            uctxt->regs[0] = KernelGetPid(uctxt);
            break;

        case YALNIX_DELAY:
            uctxt->regs[0] = KernelDelay(uctxt);
            break;

        case YALNIX_BRK:
            uctxt->regs[0] = KernelBrk(uctxt);
            break;

        case YALNIX_WAIT:
            uctxt->regs[0] = KernelWait(uctxt);
            break;
        case YALNIX_EXIT:
            uctxt->regs[0] = KernelExit(uctxt);
            break;
        default:
            TracePrintf(0, "Unknown syscall %d\n", uctxt->code);
            uctxt->regs[0] = ERROR;
            break;
    }
}