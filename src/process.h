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
#define KSTACK_PAGES (KERNEL_STACK_MAXSIZE / PAGESIZE)
// PCB: Process Control Block.
typedef struct pcb {
    int pid;
    proc_state_t state;

    // saved user and kernel context 
    UserContext user_context;
    KernelContext kernel_context;

    // Region 1 page table for current process
    pte_t *region1_pt;
    int kstack_pfn[KSTACK_PAGES];
    // parent/ child tracking
    struct pcb *parent;
    struct pcb *children;
    struct pcb *next_sibling;

    // exit and wait
    int exit_status;
    int waiting_for_child;
    int delay_ticks;

    // queue pointer
    struct pcb *next;
} pcb_t;

// current process pointer
extern pcb_t *current_process;
extern pcb_t *init_process;

// process function headers
void process_init(void);
pcb_t *process_create_idle(UserContext *uctxt);
pcb_t *process_create_child(pcb_t *parent);
void process_exit_current(int status);
pcb_t *process_wait_for_child(int *status);
pcb_t *process_create_init(UserContext *uctxt);
pcb_t *process_get_idle(void);
// scheduler function headers
pcb_t *scheduler_next(void);
void scheduler_add(pcb_t *proc);
void scheduler_block_current(void);
void scheduler_run_next(UserContext *uctxt);
pcb_t *process_create_idle(UserContext *uctxt);
pcb_t *process_wait_for_child(int *status);
KernelContext *KCSwitch(KernelContext *KCin, void *current_pcb, void *next_pcb);
KernelContext *KCCopy(KernelContext *KCin, void *new_pcb, void *not_used);
#endif