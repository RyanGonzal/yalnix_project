// Ryan Gonzalez, Sam Hirsh memory.c
#include <ykernel.h>
#include "memory.h"
//file is overall memory file

// sets up physical frame
void memory_init(unsigned int pmem_size)
{
    // compute number of physical frames from given pmem_size
    // allocateframe table
    // mark all frames as free
    // mark kernel frames as used
    // prepare for page table allocation

    (void)pmem_size;
}

// finds a free physical frame.
int alloc_frame(void)
{
    // search frame table for free frame
    // if there is no frame then return error, else 
    // mark frame as used
    // return physical frame number
    return ERROR ;
}

// releases a physical frame(sets it to free)
void free_frame(int pfn)
{
    // check that pfn is valid
    // mark frame as free
    // clear owner information
    (void)pfn;
}

//fills one page table entry
void map_page(pte_t *pt, int page_index, int pfn, int prot)
{
    // set pt[page_index].valid = 1
    // set pt[page_index].pfn = pfn
    // set pt[page_index].prot = prot

}

// invalidates one page table entry.
void unmap_page(pte_t *pt, int page_index)
{
    // mark pt[page_index] as invalid
    // flush TLB entry if needed

    (void)pt;
    (void)page_index;
}

// grows or shrinks the kernel hea
int SetKernelBrk(void *addr)
{
    // if VM is disabled:
    // allow kernel break to move if address is safe
    // else VM is enabled:
    // round addr to page boundary
    // allocate frames for new heap pages
    // map pages in Region 0 page table
    // flush TLB entries if mappings changed
    // return 0, if works or error if fails 

    (void)addr;
    return 0;
}