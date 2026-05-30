// Ryan Gonzalez, Sam Hirsh syscalls.c

#include <ykernel.h>
#include "syscalls.h"
#include "process.h"
#include "memory.h"

typedef struct tty_state {
    int write_busy;
    pcb_t *writer;
    char write_buf[TERMINAL_MAX_LINE];
    int write_len;
} tty_state_t;
static tty_state_t ttys[NUM_TERMINALS];
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
    pcb_t *child;
    child = process_create_child(current_process);
    if (child == NULL) {
        return ERROR;
    }

    return child->pid;

    
}

// replaces current prog w new prog
int KernelExec(UserContext *uctxt)
{
    // get program name and arguments from user memory
    // call LoadProgram 
    // replace current Region 1 address space
    char *name;
    char **args;
    int result;

    name = (char *)uctxt->regs[0];
    args = (char **)uctxt->regs[1];

    if (name == NULL || args == NULL) {
        return ERROR;
    }

    result = LoadProgram(name, args, current_process);

    if (result != SUCCESS) {
        return ERROR;
    }

    *uctxt = current_process->user_context;

    return SUCCESS;  

   
}

// exit current process
int KernelExit(UserContext *uctxt)
{
    // get exit status
    // mark current process as zombie
    // schedule another process
    int status;

    status = uctxt->regs[0];

    process_exit_current(status);

    current_process = NULL;

    scheduler_run_next(uctxt);

    return SUCCESS;
    

    
}

// waits for child process to exit
int KernelWait(UserContext *uctxt)
{
    // while waiting for zombie child
    // get child pid and exit status
    int *status_ptr;
    int status;
    pcb_t *child;
    int pid;

    status_ptr = (int *)uctxt->regs[0];

    child = process_wait_for_child(&status);

    if (child != NULL) {
        if (status_ptr != NULL) {
            *status_ptr = status;
        }

        pid = child->pid;
        free(child);
        return pid;
    }

    if (current_process != NULL &&
        current_process->waiting_for_child) {
        scheduler_run_next(uctxt);
        return 0;
    }

    return ERROR;
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
// int KernelTtyRead(UserContext *uctxt)
// {
//     // while data 
//     // validate user data
//     // copy input to bugger
//     int tty_id = uctxt->regs[0];
//     void *buf = uctxt->regs[1];
//     int len = uctxt->regs[2];

//     if (tty_id == NULL || buf == NULL || len == NULL) return ERROR;

//     int len_received = TtyReceive(tty_id, buf, len);

//     return received_len;
// }

// write to terminal via TTY
// write to terminal via TTY
int KernelTtyWrite(UserContext *uctxt)
{
    // if user buffer == valid
    // copy bytes to kernel buffer, call TTY transmit
    // delay until it is done 
    int tty_id;
    char *buf;
    int len;
    int chunk;

    tty_id = uctxt->regs[0];
    buf = (char *)uctxt->regs[1];
    len = uctxt->regs[2];
    //check terminal #, buf and write len
    if (tty_id < 0 || tty_id >= NUM_TERMINALS || buf == NULL || len < 0) {
        return ERROR;
    }
    //if writing 0, then nothing to write so return 
    if (len == 0) {
        return 0;
    }
    //if another tty is writing return error as only 1 tty at a time 
    if (ttys[tty_id].write_busy) {
        return ERROR;
    }
    // can only transmit max terminal line at time 
    chunk = len;
    if (chunk > TERMINAL_MAX_LINE) {
        chunk = TERMINAL_MAX_LINE;
    }
    //copy reg 1 buf into reg 0 buf
    memcpy(ttys[tty_id].write_buf, buf, chunk);
    //who is waiting for it to finish?
    ttys[tty_id].write_len = chunk;
    ttys[tty_id].writer = current_process;
    ttys[tty_id].write_busy = 1;
    
    TtyTransmit(tty_id, ttys[tty_id].write_buf, chunk);
    //block caller until TTY transmit is done
    scheduler_block_current();
    scheduler_run_next(uctxt);

    return chunk;
}
// helper to see if tty is done. this is called in traps
void tty_transmit_done(int tty_id)
{
    if (tty_id < 0 || tty_id >= NUM_TERMINALS) {
        return;
    }

    ttys[tty_id].write_busy = 0;

    if (ttys[tty_id].writer != NULL) {
        ttys[tty_id].writer->state = PROC_READY;
        scheduler_add(ttys[tty_id].writer);
        ttys[tty_id].writer = NULL;
    }
}
void syscall_handle(UserContext *uctxt)
{
    switch (uctxt->code) {
        // case YALNIX_TTY_READ:
        //     uctxt->regs[0] = KernelTtyRead(uctxt);
        //     break;
        case YALNIX_TTY_WRITE:
            uctxt->regs[0] = KernelTtyWrite(uctxt);
            break;
        case YALNIX_GETPID:
            uctxt->regs[0] = KernelGetPid(uctxt);
            break;
        case YALNIX_FORK:
            uctxt->regs[0] = KernelFork(uctxt);
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
            KernelExit(uctxt);
            break;
        case YALNIX_EXEC:
            uctxt->regs[0] = KernelExec(uctxt);
            break;
        default:
            TracePrintf(0, "Unknown syscall %d\n", uctxt->code);
            uctxt->regs[0] = ERROR;
            break;
    }
}