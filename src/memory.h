// Ryan Gonzalez, Sam Hirsh memory.h
// Header file for physical memory, page table, and kernel heap
#ifndef MEMORY_H
#define MEMORY_H
#include <ykernel.h>

struct pcb;
typedef struct pcb pcb_t;
// Physical frame states
typedef enum frame_state {
    FRAME_FREE,
    FRAME_KERNEL,
    FRAME_USER,
    FRAME_PAGETABLE,
    FRAME_KSTACK
} frame_state_t;

// Frame table entry
typedef struct frame_info {
    frame_state_t state;
    int owner_pid;
} frame_info_t;

// memory setup
void memory_init(unsigned int pmem_size);
void memory_init_region0(void);
void memory_init_idle_region1(void);
void memory_enable_vm(void);

// frame helpers
int alloc_frame(void);
void free_frame(int pfn);

// page table
void map_page(pte_t *pt, int page_index, int pfn, int prot);
void unmap_page(pte_t *pt, int page_index);
pte_t *memory_get_region1_pt(void);
void *memory_get_idle_stack_top(void);

int SetKernelBrk(void *addr);
// Region mem
pte_t *memory_init_region1(void);

// Load user prograsm into a process address space
int LoadProgram(char *name, char *args[], pcb_t *proc);
int memory_alloc_kstack(pcb_t *proc);
void memory_save_current_kstack(pcb_t *proc);
void memory_restore_kstack(pcb_t *proc);
void memory_copy_kstack_page(int vpn, int pfn);
int memory_copy_region1(pte_t *parent_pt, pte_t *child_pt);
void memory_capture_boot_kstack(pcb_t *proc);

#endif