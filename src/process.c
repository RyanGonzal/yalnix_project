// Ryan Gonzalez, Sam Hirsh process.c
#include <ykernel.h>
#include "process.h"
#include "memory.h"
// file is process handling( pcb) file 
// current handling process
pcb_t *current_process = NULL;
pcb_t *init_process = NULL;
static pcb_t idle_pcb;
static pcb_t init_pcb;

typedef struct {
    pcb_t *front;
    pcb_t *back;
} pcb_queue_t;
static pcb_queue_t ready_queue;
static pcb_queue_t blocked_queue;




void init_queue(pcb_queue_t *rq) {
    rq->front = NULL;
    rq->back = NULL;
}

void enqueue(pcb_queue_t *rq, pcb_t *pcb) {
    pcb->next = NULL;

    if (rq->back != NULL) rq->back->next = pcb;

    rq->back = pcb;

    if (rq->front == NULL) rq->front = pcb;
}

pcb_t *dequeue(pcb_queue_t *rq) {
    if (rq->front == NULL) return NULL;

    pcb_t *dequeued = rq->front;
    rq->front = rq->front->next;

    if (rq->front == NULL) rq->back = NULL;

    dequeued->next = NULL;

    return dequeued;
}


// process_init sets up global process structure
void process_init(void) {
    // // initialize global process table / queue structure
    // // initialize ready queue
    // // initialize blocked queues

    // pcb_queue_t *ready_queue = malloc(sizeof(pcb_queue_t));
    // if (ready_queue == NULL) return ERROR;
    
    // pcb_queue_t *blocked_queue = malloc(sizeof(pcb_queue_t));
    // if (blocked_queue == NULL) return ERROR;

    // // set current_process to NULL
    // // prepare for idle
    // //  PCB creation
    // // current_process = NULL;
    // // init_process = NULL;

    // //  PCB creation
    // // Nothing below this is correct
    // pcb_t new_pcb = malloc(sizeof(new_pcb_t));
    // if (new_pcb == NULL) return ERROR;  

    // // new_pcb.region1_pt = memory_get_region1_pt();
    // // new_pcb.pid = helper_new_pid(idle_pcb.region1_pt);
    // // new_pcb.state = PROC_READY;
    // // new_pcb.region1_pt = memory_get_region1_pt();
    // // new_pcb.parent = NULL;
    // // new_pcb.children = NULL;
    // // new_pcb.next_sibling = NULL;
    // // new_pcb.next = NULL;
    // // new_pcb.exit_status = 0;
    // // new_pcb.waiting_for_child = 0;

    // // save current user context
    // UserContext *uctxt = malloc(sizeof(UserContext))
    // if (uctxt == NULL) return ERROR;  
    // // Allocate kernel context
    // KernelContext *kntxt = malloc(sizeof(KernelContext))
    // // add kernel stack frames

    // // new_pcb.user_context = *uctxt;


    // // prepare for idle
    // // we dont need to create a pcb, do we even need this function?
    current_process = NULL;
    init_process = NULL;

    init_queue(&ready_queue);
    init_queue(&blocked_queue);
}

// process_create_idle creates the idle process
pcb_t *process_create_idle(UserContext *uctxt)
{
    idle_pcb.region1_pt = memory_get_region1_pt();
    idle_pcb.pid = helper_new_pid(idle_pcb.region1_pt);
    idle_pcb.state = PROC_RUNNING;
    idle_pcb.kernel_context_valid = 0;

    //page table 
    idle_pcb.region1_pt = memory_get_region1_pt();

    // dle has no parent, child or states
    idle_pcb.parent = NULL;
    idle_pcb.children = NULL;
    idle_pcb.next_sibling = NULL;
    idle_pcb.next = NULL;
    idle_pcb.exit_status = 0;
    idle_pcb.waiting_for_child = 0;
    idle_pcb.delay_ticks = 0;

    //save current user context
    idle_pcb.user_context = *uctxt;

    // Allocate kernel context
    // KernelContext *kntxt = malloc(sizeof(KernelContext));
    int base_page = KERNEL_STACK_BASE >> PAGESHIFT;
    memory_capture_boot_kstack(&idle_pcb);

   

    // create initial kernel context for idle
    // add kernel stack frames

    //set process to idle
    current_process = &idle_pcb;

    return &idle_pcb;
}
pcb_t *process_create_init(UserContext *uctxt)
{
    init_pcb.kernel_context_valid = 0;
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
    if (memory_alloc_kstack(&init_pcb) == ERROR) {
        helper_abort("process_create_init: kstack allocation failed");
    }
   
    init_process = &init_pcb;
    scheduler_add(&init_pcb);
    return &init_pcb;
}


// process_create_child creates a child process for Fork
pcb_t *process_create_child(pcb_t *parent)
{
    pcb_t *child;

    if (parent == NULL) {
        return NULL;
    }

    // allocate PCB for child
    child = malloc(sizeof(pcb_t));

    if (child == NULL) {
        return NULL;
    }
    // allocate child Region 1 page table
    child->region1_pt = memory_init_region1();

    if (child->region1_pt == NULL) {
        free(child);
        return NULL;
    }
    // copy valid parent pages into child address space
    if (memory_copy_region1(parent->region1_pt, child->region1_pt) == ERROR) {
        free(child);
        return NULL;
    }
    // give child its own kernel stack frames
    if (memory_alloc_kstack(child) == ERROR) {
        free(child);
        return NULL;
    }

    // assign new PID
    child->pid = helper_new_pid(child->region1_pt);
    child->state = PROC_READY;

    // copy parent kernel context and stack to child
    if (KernelContextSwitch(KCCopy, child, NULL) == ERROR) {
        free(child);
        return NULL;
    } 
    child->kernel_context_valid = 1;

    // copy parent UserContext
    child->user_context = parent->user_context;
    // parent returns child pid, child returns 0
    child->user_context.regs[0] = 0;
    parent->user_context.regs[0] = child->pid;

    // link child to parent
    child->parent = parent;
    child->children = NULL;
    child->next_sibling = parent->children;
    parent->children = child;
    child->next = NULL;
    child->exit_status = 0;
    child->waiting_for_child = 0;
    child->delay_ticks = 0;
    // add child to ready queue
    scheduler_add(child);
    return child;
}


// process_exit_current marks current process as exited.
void process_exit_current(int status)
{
    // save exit status
    // mark current process as ZOMBIE
    // wake parent if parent is waiting
    // schedule next process
    pcb_t *proc = current_process;

    if (proc == NULL) {
        return;
    }

    proc->exit_status = status;
    proc->state = PROC_ZOMBIE;

    if (proc->parent != NULL && proc->parent->waiting_for_child) {
        proc->parent->waiting_for_child = 0;
        proc->parent->state = PROC_READY;
        scheduler_add(proc->parent);
    }
}

// scheduler_next chooses the next process to run.
// scheduler_next chooses the next process to run.
pcb_t *scheduler_next(void)
{
    // if ready queue has a process, return first process in ready queue
    // else return idle process

    //for now if init ready run init, else idle 
    // if (init_process != NULL && init_process->state == PROC_READY) {
    //     return init_process;
    // }
    // return &idle_pcb;
    //above was cp3
    pcb_t *next;

    while ((next = dequeue(&ready_queue)) != NULL) {
        if (next->state == PROC_READY) {
            return next;
        }
    }

    return process_get_idle();
}

// scheduler_add adds a process to the ready queue.
void scheduler_add(pcb_t *proc) {
    // mark process as READY
    // add process to ready queue
    if (proc == NULL || proc == process_get_idle()) {
        return;
    }

    proc->state = PROC_READY;
    enqueue(&ready_queue, proc);



    (void)proc;
}

// scheduler_block_current blocks the currently running process.
void scheduler_block_current(void) {
    // mark current process as BLOCKED
    // place current process on correct blocked queue
    // schedule another process
    if (current_process != NULL) {
        current_process->state = PROC_BLOCKED;
    }
}
void scheduler_run_next(UserContext *uctxt)
{
    // save current UserContext into current_process PCB
    // choose next process using scheduler_next
    // switch Region 1 page table to next process
    // switch kernel stack mapping if needed
    // flush TLB entries
    // copy next process UserContext into uctxt
    pcb_t *old;
    pcb_t *next;

    old = current_process;

    // Save current process user context
    if (old != NULL) {
        old->user_context = *uctxt;

        if (old != process_get_idle() &&
            old->state == PROC_RUNNING) {
            old->state = PROC_READY;
            scheduler_add(old);
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
    // Switch kernel stack/context safely
    TracePrintf(0,
        "SCHED oldpid=%d oldvalid=%d nextpid=%d nextvalid=%d oldstk=%d,%d nextstk=%d,%d\n",
        old ? old->pid : -1,
        old ? old->kernel_context_valid : -1,
        current_process->pid,
        current_process->kernel_context_valid,
        old ? old->kstack_pfn[0] : -1,
        old ? old->kstack_pfn[1] : -1,
        current_process->kstack_pfn[0],
        current_process->kstack_pfn[1]);

    if (old != NULL && old != current_process) {
        if (old->kernel_context_valid &&
            current_process->kernel_context_valid) {
            if (KernelContextSwitch(KCSwitch, old, current_process) == ERROR) {
                helper_abort("scheduler_run_next: KernelContextSwitch failed");
            }
        } else {
            memory_restore_kstack(current_process);
        }
    } else if (old == NULL) {
        memory_restore_kstack(current_process);
    }

    // Restore selected process user context
    *uctxt = current_process->user_context;
}
pcb_t *process_get_idle(void)
{
    return &idle_pcb;
}
//helper for wait and syswait
static pcb_t *find_zombie_child(pcb_t *parent)
{
    //return if there is a zombie, else NULL
    pcb_t *child = parent->children;
    
    while (child != NULL) {
        if (child->state == PROC_ZOMBIE) {
            return child;
        }
        child = child->next_sibling;
    }
    return NULL;
}
//remove a child from parent, given target 
static void remove_child(pcb_t *parent, pcb_t *target)
{
    pcb_t *prev = NULL;
    pcb_t *curr = parent->children;
    // while curr exits
    while (curr != NULL) {
        // if found switch child to next sib and mark to parent
        if (curr == target) {
            if (prev == NULL) {
                parent->children = curr->next_sibling;
            } else {
                prev->next_sibling = curr->next_sibling;
            }
            curr->next_sibling = NULL;
            return;
        }
        //this is leet code at its finest
        prev = curr;
        curr = curr->next_sibling;
    }
}
// wait needs to have 3 cases, no children then error, zombie child reap, or has running children so block parent 
pcb_t *process_wait_for_child(int *status)
{
    pcb_t *parent = current_process;
    pcb_t *zombie;
    // if no parent or children just return 
    if (parent == NULL) {
        return NULL;
    }
    if (parent->children == NULL) {
        return NULL;
    }
    zombie = find_zombie_child(parent);
    // if there is zombie child, return status and remove zombie
    if (zombie != NULL) {
        if (status != NULL) {
            *status = zombie->exit_status;
        }

        remove_child(parent, zombie);
        // also remove return so parent can maybe be unblocked
        return zombie;
    }
    // else block 
    parent->waiting_for_child = 1;
    scheduler_block_current();

    return NULL;
}