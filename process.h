// Ryan Gonzalez, Sam Hirsh process.h
// Header file for process structs and process function prototypes
#ifndef PROCESS_H
#define PROCESS_H

#include <ykernel.h>

// Process states
typedef enum proc_state {
    PROC_READY,
    PROC_RUNNING,
    PROC_BLOCKED,
    PROC_ZOMBIE
} proc_state_t;

// PCB: Process Control Block.
typedef struct pcb {
    int pid;

    
    proc_state_t state;

    // saved user and kernel context 
    UserContext user_context;
    KernelContext kernel_context;

    // Region 1 page table for current process
    pte_t *region1_pt;

    // parent/ child tracking
    struct pcb *parent;
    struct pcb *children;
    struct pcb *next_sibling;

    // exit and wait
    int exit_status;
    int waiting_for_child;

    // queue pointer
    struct pcb *next;
} pcb_t;

// current process pointer
extern pcb_t *current_process;

// process function headers
void process_init(void);
pcb_t *process_create_idle(UserContext *uctxt);
pcb_t *process_create_child(pcb_t *parent);
void process_exit_current(int status);
pcb_t *process_wait_for_child(int *status);

// scheduler function headers
pcb_t *scheduler_next(void);
void scheduler_add(pcb_t *proc);
void scheduler_block_current(void);
void scheduler_run_next(UserContext *uctxt);

#endif