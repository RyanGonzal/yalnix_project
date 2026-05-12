// Ryan Gonzalez, Sam Hirsh memory.c
#include <ykernel.h>
#include "memory.h"
//file is overall memory file

// frame
static frame_info_t *frame_table = NULL;
static int num_frames = 0;

// page table
static pte_t *region0 = NULL;
static pte_t *idle_region1 = NULL;

// stack top
static void *idle_stack_top = NULL;

// VM flag and kernel break tracking
static int vm_enabled = 0;
static int kernel_brk_p = 0;

// sets up physical frame
void memory_init(unsigned int pmem_size)
{
    // number of physical frames
    num_frames = pmem_size >> PAGESHIFT;

    frame_table = malloc(num_frames * sizeof(frame_info_t));

    //set all tables as free
    for (int i = 0; i < num_frames; i++){
        frame_table[i].state = FRAME_FREE;
        frame_table[i].owner_pid = -1;
    }
    kernel_brk_p = _orig_kernel_brk_page;

    for (int i = _first_kernel_text_page; i < _orig_kernel_brk_page; i++){
        frame_table[i].state = FRAME_KERNEL;
        frame_table[i].owner_pid = 0;
    }
}
    

// finds a free physical frame.
int alloc_frame(void)
{
    // search frame table for free frame
    for (int i = 0; i < num_frames; i++){
        if(frame_table[i].state == FRAME_FREE){
            frame_table[i].state = FRAME_USER;
            frame_table[i].owner_pid = -1;
            return i;
        }
    }
    //if no frames are free
    return ERROR ;
}

// releases a physical frame(sets it to free)
void free_frame(int pfn)
{
    // check that pfn is valid
    if( pfn < 0 || pfn >= num_frames){
        return;
    }

    frame_table[pfn].state = FRAME_FREE;
    frame_table[pfn].owner_pid = -1;
    
}

//fills one page table entry
void map_page(pte_t *pt, int page_index, int pfn, int prot)
{
     pt[page_index].valid = 1;
     pt[page_index].pfn = pfn;
     pt[page_index].prot = prot;

}

// invalidates one page table entry.
void unmap_page(pte_t *pt, int page_index)
{
    pt[page_index].valid = 0;
}
void memory_init_idle_region1(void){
    int stack_frame;
    int stack_page;

    idle_region1 = malloc(VMEM_1_SIZE / PAGESIZE * sizeof(pte_t));

    // invalidate all Region 1 pages
    for (int i = 0; i < VMEM_1_SIZE / PAGESIZE; i++) {
        idle_region1[i].valid = 0;
    }
    // set valid stack page for region 1 and mark as user 
    stack_page = (VMEM_1_LIMIT - VMEM_1_BASE) / PAGESIZE - 1;
    stack_frame = alloc_frame();
    frame_table[stack_frame].state = FRAME_USER;
    frame_table[stack_frame].owner_pid = 0;
    //idle
    map_page(idle_region1, stack_page, stack_frame, PROT_READ | PROT_WRITE);
    idle_stack_top = (void *)VMEM_1_LIMIT;


}
void memory_init_region0(void)
{
    region0 = malloc(VMEM_0_SIZE / PAGESIZE * sizeof(pte_t));

    //invalidation
    for(int i = 0; i <VMEM_0_SIZE/ PAGESIZE; i++){
        region0[i].valid = 0;
    } 
    for(int i = _first_kernel_text_page; i < _first_kernel_data_page; i++){
        map_page(region0, i, i, PROT_READ | PROT_EXEC);
    }

    for(int i = _first_kernel_data_page; i < kernel_brk_p; i++){
        map_page(region0, i, i, PROT_READ | PROT_WRITE);
    }
    // map kernel stack page
    for(int i = KERNEL_STACK_BASE >> PAGESHIFT; i < KERNEL_STACK_LIMIT >> PAGESHIFT; i++){
        map_page(region0, i, i, PROT_READ | PROT_WRITE);
    }
  
}
void memory_enable_vm(void)
{
    // set Region 0 page table regs
    WriteRegister(REG_PTBR0, (unsigned int)region0);
    WriteRegister(REG_PTLR0, VMEM_0_SIZE / PAGESIZE);
    // set Region 1 page table regs
    WriteRegister(REG_PTBR1, (unsigned int)idle_region1);
    WriteRegister(REG_PTLR1, VMEM_1_SIZE / PAGESIZE);
    WriteRegister(REG_VM_ENABLE, 1);
    vm_enabled = 1;
}
pte_t *memory_get_region1_pt(void)
{
    return idle_region1;
}
void *memory_get_idle_stack_top(void)
{
    return (void *)(VMEM_1_LIMIT - 4);
}


// grows or shrinks the kernel hea
int SetKernelBrk(void *addr)
{
    int new_brk_page;
    int frame;

    //conversion
    new_brk_page = UP_TO_PAGE(addr) >> PAGESHIFT;

    // if disable just track kernel growth
    if (!vm_enabled) {
        if (new_brk_page > kernel_brk_p) {
            kernel_brk_p = new_brk_page;
        }
        return 0;
    }

    //once has been enabled track pages
    for (int i = kernel_brk_p; i < new_brk_page; i++) {
        frame = alloc_frame();

        if (frame == ERROR) {
            return ERROR;
        }

        frame_table[frame].state = FRAME_KERNEL;
        frame_table[frame].owner_pid = 0;

        map_page(region0, i, frame, PROT_READ | PROT_WRITE);
    }

    kernel_brk_p = new_brk_page;
  

    return 0;
}