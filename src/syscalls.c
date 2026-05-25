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
    // if parent is waiting wake, reap
    // schedule another process

    (void)uctxt;
    return ERROR;
}

// waits for child process to exit
int KernelWait(UserContext *uctxt)
{
    // while waiting for zombie child
    // get child pid and exit status

    (void)uctxt;
    return ERROR;
}

// get current pid
int KernelGetPid(UserContext *uctxt)
{
    // return current_process pid

    (void)uctxt;
    return ERROR;
}

// changes heap break 
int KernelBrk(UserContext *uctxt)
{
    // if break address is validated
    // grow or shrink, then update page table
    (void)uctxt;
    return ERROR;
}

// delays kernel by blocking clock
int KernelDelay(UserContext *uctxt)
{
    // get delay ticks
    // block current process
    // wake after delay

    (void)uctxt;
    return ERROR;
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
