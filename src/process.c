// Ryan Gonzalez, Sam Hirsh process.c
#include <ykernel.h>
#include "process.h"
#include "memory.h"
// file is process handling( pcb) file 
// current handling process
pcb_t *current_process = NULL;
static pcb_t idle_pcb;




// process_init sets up global process structure
void process_init(void)
{
    // initialize global process table / queue structure
    // initialize ready queue
    // initialize blocked queues
    // set current_process to NULL
    // prepare for idle
    //  PCB creation
    current_process = NULL;
}

// process_create_idle creates the idle process
pcb_t *process_create_idle(UserContext *uctxt)
{
    idle_pcb.region1_pt = memory_get_region1_pt();
    idle_pcb.pid = helper_new_pid(idle_pcb.region1_pt);
    idle_pcb.state = PROC_RUNNING;

    //page table 
    idle_pcb.region1_pt = memory_get_region1_pt();

    // dle has no parent, child or states
    idle_pcb.parent = NULL;
    idle_pcb.children = NULL;
    idle_pcb.next_sibling = NULL;
    idle_pcb.next = NULL;
    idle_pcb.exit_status = 0;
    idle_pcb.waiting_for_child = 0;

    //save current user context
    idle_pcb.user_context = *uctxt;

    //set process to idle
    current_process = &idle_pcb;

    return &idle_pcb;
}

// process_create_child creates a child process for Fork
pcb_t *process_create_child(pcb_t *parent)
{
    // allocate PCB for child
    // assign new PID
    // copy parent UserContext
    // allocate child Region 1 page table
    // copy valid parent pages into child address space
    // link child to parent
    // add child to ready queue

    (void)parent;
    return NULL;
}

// process_exit_current marks current process as exited.
void process_exit_current(int status)
{
    // save exit status
    // mark current process as ZOMBIE
    // wake parent if parent is waiting
    // schedule next process
    pcb_t *proc = current_process;
    proc->exit_status = status;
    proc->state = PROC_ZOMBIE;
    if (proc->parent != NULL && proc->parent->waiting_for_child) {
        proc->parent->waiting_for_child = 0;
        scheduler_add(proc->parent);
    }
}

// scheduler_next chooses the next process to run.
pcb_t *scheduler_next(void)
{
    // if ready queue has a process, return first process in ready queue
    // else return idle process

    return current_process;
}

// scheduler_add adds a process to the ready queue.
void scheduler_add(pcb_t *proc)
{
    // mark process as READY
    // add process to ready queue

    (void)proc;
}

// scheduler_block_current blocks the currently running process.
void scheduler_block_current(void)
{
    // mark current process as BLOCKED
    // place current process on correct blocked queue
    // schedule another process
}

// scheduler_run_next switches from current process to next process
void scheduler_run_next(UserContext *uctxt)
{
    // save current UserContext into current_process PCB
    // choose next process using scheduler_next
    // switch Region 1 page table to next process
    // switch kernel stack mapping if needed
    // flush TLB entries
    // copy next process UserContext into uctxt

    (void)uctxt;
}