// Ryan Gonzalez, Sam Hirsh process.c
#include <ykernel.h>
#include "process.h"
#include "memory.h"
// file is process handling( pcb) file 
// current handling process
pcb_t *current_process = NULL;
static pcb_t idle_pcb;
static pcb_t init_pcb;
pcb_t *init_process = NULL;

typedef struct {
    pcb_t front;
    pcb_t back;
} pcb_queue_t;

void init_queue(pcb_queue_t *rq) {
    rq->front = NULL;
    rq->back = NULL;
}

void enqueue(pcb_queue_t *rq, pcb_t pcb) {
    pcb->next = NULL;

    if (rq->back != NULL) rq->back->next = pcb;

    rq->back = pcb;

    if (rq->front != NULL) rq->front = pcb;
}

void dequeue(pcb_queue_t *rq, pcb_t pcb) {
    if (rq->front == NULL) return 0;

    pcb_t dequeued = rq->front;

    rq->front = rq->front->next;

    if (rq->front == NULL) rq->front = rq->back;
}


// process_init sets up global process structure
void process_init(void) {
    // initialize global process table / queue structure
    // initialize ready queue
    // initialize blocked queues

    pcb_queue_t *ready_queue = malloc(sizeof(pcb_queue_t));
    if (ready_queue == NULL) return ERROR;
    
    pcb_queue_t *blocked_queue = malloc(sizeof(pcb_queue_t));
    if (blocked_queue == NULL) return ERROR;

    // set current_process to NULL
    // prepare for idle
    //  PCB creation
    current_process = NULL;
    init_process = NULL;

    //  PCB creation
    // Nothing below this is correct
    pcb_t new_pcb = malloc(sizeof(new_pcb_t));
    if (new_pcb == NULL) return ERROR;  

    new_pcb.region1_pt = memory_get_region1_pt();
    new_pcb.pid = helper_new_pid(idle_pcb.region1_pt);
    new_pcb.state = PROC_READY;
    new_pcb.region1_pt = memory_get_region1_pt();
    new_pcb.parent = NULL;
    new_pcb.children = NULL;
    new_pcb.next_sibling = NULL;
    new_pcb.next = NULL;
    new_pcb.exit_status = 0;
    new_pcb.waiting_for_child = 0;

    // save current user context
    UserContext *uctxt = malloc(sizeof(UserContext))
    if (uctxt == NULL) return ERROR;  

    new_pcb.user_context = *uctxt;

    enqueue(ready_queue, new_pcb);

    // prepare for idle
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
pcb_t *process_create_init(UserContext *uctxt)
{
    init_pcb.region1_pt = memory_init_region1();
    if (init_pcb.region1_pt == NULL) {
        helper_abort("failed to create Region 1 page table");
    }

    init_pcb.pid = helper_new_pid(init_pcb.region1_pt);
    init_pcb.state = PROC_READY;
    //init has no real states
    init_pcb.parent = NULL;
    init_pcb.children = NULL;
    init_pcb.next_sibling = NULL;
    init_pcb.next = NULL;
    init_pcb.exit_status = 0;
    init_pcb.waiting_for_child = 0;
    init_pcb.delay_ticks = 0;

    init_pcb.user_context = *uctxt;

    init_process = &init_pcb;

    return &init_pcb;
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

    //for now if init ready run init, else idle 
    if (init_process != NULL && init_process->state == PROC_READY) {
        return init_process;
    }

    return &idle_pcb;

}

// scheduler_add adds a process to the ready queue.
void scheduler_add(pcb_t *proc)
{
    // mark process as READY
    // add process to ready queue
    //for now just set to ready
    if (proc == NULL) {
        return;
    }
    proc->state = PROC_READY;
}

// scheduler_block_current blocks the currently running process.
void scheduler_block_current(void)
{
    // mark current process as BLOCKED
    // place current process on correct blocked queue
    // schedule another process
     if (current_process != NULL) {
        current_process->state = PROC_BLOCKED;
    }
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
    pcb_t *next;

    // Save current process user context
    if (current_process != NULL) {
        current_process->user_context = *uctxt;

        if (current_process->state == PROC_RUNNING) {
            current_process->state = PROC_READY;
        }
    }
    // Pick next process
    next = scheduler_next();

    current_process = next;
    current_process->state = PROC_RUNNING;

    // Switch Region 1 to selected process
    WriteRegister(REG_PTBR1, (unsigned int)current_process->region1_pt);
    WriteRegister(REG_TLB_FLUSH, TLB_FLUSH_1);

    // Restore selected process user context
    *uctxt = current_process->user_context;   
}
pcb_t *process_get_idle(void)
{
    return &idle_pcb;
}