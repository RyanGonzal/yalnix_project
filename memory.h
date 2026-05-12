// Ryan Gonzalez, Sam Hirsh memory.h
// Header file for physical memory, page table, and kernel heap
#ifndef MEMORY_H
#define MEMORY_H
#include <ykernel.h>

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

// memory functions
void memory_init(unsigned int pmem_size);
int alloc_frame(void);
void free_frame(int pfn);

// page table headers
void map_page(pte_t *pt, int page_index, int pfn, int prot);
void unmap_page(pte_t *pt, int page_index);

// kernel heap function
int SetKernelBrk(void *addr);

#endif