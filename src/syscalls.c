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
    int tty_id = regs[0];
    void *buf = regs[1];
    int len = regs[2];

    if (tty_id == NULL || buf == NULL || len == NULL) return ERROR;

    int len_received = TtyReceive(tty_id, buf, len);

    return received_len;
}

// write to terminal via TTY
int KernelTtyWrite(UserContext *uctxt)
{
    // if user buffer == valid
    // copt bytes to kernel buffer, call TTY transmit
    // delay until it is done 
    int tty_id = regs[0];
    void *buf = regs[1];
    int len = regs[2];

    if (tty_id == NULL || buf == NULL || len == NULL) return ERROR;

    // pseudo ish code, treat as "notes" for now
    char *transmit_line[TERMINAL_MAX_LINE];
    for (int i = 0; i < len / TERMINAL_MAX_LINE; i++) {
        memcpy(&transmit_line, &(char*)buf[i*TERMINAL_MAX_LINE], TERMINAL_MAX_LINE)
        TtyTransmit(tty_id, max_line, len);
    }
    
    int remaining_byes = len % TERMINAL_MAX_LINE;
    if (len % TERMINAL_MAX_LINE != 0) {
        memcpy(&transmit_line, &(char*)buf[i*TERMINAL_MAX_LINE], remaining_byes)
        TtyTransmit(tty_id, buf, len);
    }

    return len;
}
void syscall_handle(UserContext *uctxt)
{
    // handle function just delaying and switching 
    switch (uctxt->code) {
        case YALNIX_TTY_READ:
            uctxt->regs[0] = KernelTtyRead(uctxt);
            break;
        case YALNIX_TTY_WRITE:
            uctxt->regs[0] = KernelTtyRead(uctxt);
            break;
        case YALNIX_GETPID:
            uctxt->regs[0] = current_process->pid;
            break;
        case YALNIX_DELAY:
            if (uctxt->regs[0] <= 0) {
                uctxt->regs[0] = ERROR;
                break;
            }
            current_process->delay_ticks = uctxt->regs[0];
            scheduler_block_current();
            scheduler_run_next(uctxt);
            break;
        case YALNIX_BRK:
            uctxt->regs[0] = 0;
            break;
        default:
            TracePrintf(0, "Unknown syscall %d\n", uctxt->code);
            uctxt->regs[0] = ERROR;
            break;
    }
}