// Ryan Gonzalez, Sam Hirsh memory.c
#include <ykernel.h>
#include "memory.h"
#include <fcntl.h>
#include <unistd.h>
#include <load_info.h>
#include "process.h"
//file is overall memory file

// frame
static frame_info_t *frame_table = NULL;
static int num_frames = 0;

// page table
static pte_t *region0 = NULL;
static pte_t *region1 = NULL;

static pte_t *idle_region1 = NULL;

// stack top
static void *idle_stack_top = NULL;

// VM flag and kernel break tracking
static int vm_enabled = 0;
static int kernel_brk_p = 0;

int text_pg1;
int data_pg1;
int data_npg;

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
            frame_table[i].owner_pid = -1; // TODO: set to pid
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
    if (region0 == NULL) {
        helper_abort("memory_init_region0: malloc failed");
    }

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

pte_t *memory_init_region1(void) {
    // region1 = malloc(MAX_PT_LEN * * sizeof(pte_t))

    // text_pg1 = (li.t_vaddr - VMEM_1_BASE) >> PAGESHIFT;
    // data_pg1 = (li.id_vaddr - VMEM_1_BASE) >> PAGESHIFT;
    // data_npg = li.id_npg + li.ud_npg;

    // for (i = text_pg1; i < text_pg1 + li.t_npg - 1) {
    //     map_page(region1, i, i, PROT_READ | PROT_WRITE);
    // }
    // for (i = data_pg1; i < data_pg1 + data_npg - 1) {
    //     map_page(region1, i, i, PROT_READ | PROT_WRITE);
    // }
    // for (i = MAX_PT_LEN - stack_npg; i < MAX_PT_LEN -1) {
    //     map_page(region1, i, i, PROT_READ | PROT_WRITE);
    // }
    // this function shoud just init and not map 

    pte_t *pt = malloc(MAX_PT_LEN * sizeof(pte_t));
    if (pt == NULL) {
        return NULL;
    }

    for (int i = 0; i < MAX_PT_LEN; i++) {
        pt[i].valid = 0;
        pt[i].prot = 0;
        pt[i].pfn = 0;
    }

    return pt;
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



/*
 * ==>> #include anything you need for your kernel here
 */

/*
 *  Load a program into an existing address space.  The program comes from
 *  the Linux file named "name", and its arguments come from the array at
 *  "args", which is in standard argv format.  The argument "proc" points
 *  to the process or PCB structure for the process into which the program
 *  is to be loaded. 
 */

/*
 * ==>> Declare the argument "proc" to be a pointer to the PCB of 
 * ==>> the current process. 
 */
int LoadProgram(char *name, char *args[], pcb_t *proc) {
    int fd;
    int (*entry)();
    struct load_info li;
    int i;
    char *cp;
    char **cpp;
    char *cp2;
    int argcount;
    int size;
    int text_pg1;
    int data_pg1;
    int data_npg;
    int stack_npg;
    long segment_size;
    char *argbuf;


    /*
    * Open the executable file 
    */
    if ((fd = open(name, O_RDONLY)) < 0) {
        TracePrintf(0, "LoadProgram: can't open file '%s'\n", name);
        return ERROR;
    }

    if (LoadInfo(fd, &li) != LI_NO_ERROR) {
        TracePrintf(0, "LoadProgram: '%s' not in Yalnix format\n", name);
        close(fd);
        return (-1);
    }

    if (li.entry < VMEM_1_BASE) {
        TracePrintf(0, "LoadProgram: '%s' not linked for Yalnix\n", name);
        close(fd);
        return ERROR;
    }

    /*
    * Figure out in what region 1 page the different program sections
    * start and end
    */
    text_pg1 = (li.t_vaddr - VMEM_1_BASE) >> PAGESHIFT;
    data_pg1 = (li.id_vaddr - VMEM_1_BASE) >> PAGESHIFT;
    data_npg = li.id_npg + li.ud_npg;
    /*
    *  Figure out how many bytes are needed to hold the arguments on
    *  the new stack that we are building.  Also count the number of
    *  arguments, to become the argc that the new "main" gets called with.
    */
    size = 0;
    for (i = 0; args[i] != NULL; i++) {
        TracePrintf(3, "counting arg %d = '%s'\n", i, args[i]);
        size += strlen(args[i]) + 1;
    }
    argcount = i;

    TracePrintf(2, "LoadProgram: argsize %d, argcount %d\n", size, argcount);

    /*
    *  The arguments will get copied starting at "cp", and the argv
    *  pointers to the arguments (and the argc value) will get built
    *  starting at "cpp".  The value for "cpp" is computed by subtracting
    *  off space for the number of arguments (plus 3, for the argc value,
    *  a NULL pointer terminating the argv pointers, and a NULL pointer
    *  terminating the envp pointers) times the size of each,
    *  and then rounding the value *down* to a double-word boundary.
    */
    cp = ((char *)VMEM_1_LIMIT) - size;

    cpp = (char **)
        (((int)cp - 
        ((argcount + 3 + POST_ARGV_NULL_SPACE) *sizeof (void *))) & ~7);

    /*
    * Compute the new stack pointer, leaving INITIAL_STACK_FRAME_SIZE bytes
    * reserved above the stack pointer, before the arguments.
    */
    cp2 = (caddr_t)cpp - INITIAL_STACK_FRAME_SIZE;



    TracePrintf(1, "prog_size %d, text %d data %d bss %d pages\n",
    li.t_npg + data_npg, li.t_npg, li.id_npg, li.ud_npg);


    /* 
    * Compute how many pages we need for the stack */
    stack_npg = (VMEM_1_LIMIT - DOWN_TO_PAGE(cp2)) >> PAGESHIFT;

    TracePrintf(1, "LoadProgram: heap_size %d, stack_size %d\n",
    li.t_npg + data_npg, stack_npg);


    /* leave at least one page between heap and stack */
    if (stack_npg + data_pg1 + data_npg >= MAX_PT_LEN) {
        close(fd);
        return ERROR;
    }

    /*
    * This completes all the checks before we proceed to actually load
    * the new program.  From this point on, we are committed to either
    * loading succesfully or killing the process.
    */

    /*
    * Set the new stack pointer value in the process's UserContext
    */

    /* 
    * ==>> (rewrite the line below to match your actual data structure) 
    * ==>> proc->uc.sp = cp2; 
    */
    proc->user_context.sp = cp2;
    proc->user_context.sp = cp2;

    /*
    * Now save the arguments in a separate buffer in region 0, since
    * we are about to blow away all of region 1.
    */
    cp2 = argbuf = (char *)malloc(size);

    /* 
    * ==>> You should perhaps check that malloc returned valid space 
    */

    if (cp2 == NULL) {
        //if one fails more will fail so cant do trace print and continue 
        close(fd);
        return ERROR;
    }

    for (i = 0; args[i] != NULL; i++) {
        TracePrintf(3, "saving arg %d = '%s'\n", i, args[i]);
        strcpy(cp2, args[i]);
        cp2 += strlen(cp2) + 1;
    }

    /*
    * Set up the page tables for the process so that we can read the
    * program into memory.  Get the right number of physical pages
    * allocated, and set them all to writable.
    */

    /* ==>> Throw away the old region 1 virtual address space by
    * ==>> curent process by walking through the R1 page table and,
    * ==>> for every valid page, free the pfn and mark the page invalid.
    */

    // for (int i = VMEM_1_BASE / PAGESIZE; i < VMEM_1_LIMIT / PAGESIZE; i++) {
    //     proc->region1_pt[i].valid = 0;
    //     free_frame(proc->region1_pt[i]);
    // }
    //region1 pt is just to max len, not vMEM
    for (i = 0; i < MAX_PT_LEN; i++) {
        if (proc->region1_pt[i].valid) {
            // free frame needed .pfn
            free_frame(proc->region1_pt[i].pfn);
            //needs to fully clear it, wondering should setting valid, prot and pfn to 0 be done in free frame?
            proc->region1_pt[i].valid = 0;
            proc->region1_pt[i].prot = 0;
            proc->region1_pt[i].pfn = 0;
        }
    }


    /*
    * ==>> Then, build up the new region1.  
    * ==>> (See the LoadProgram diagram in the manual.)
    */


    /*
    * ==>> First, text. Allocate "li.t_npg" physical pages and map them starting at
    * ==>> the "text_pg1" page in region 1 address space.
    * ==>> These pages should be marked valid, with a protection of
    * ==>> (PROT_READ | PROT_WRITE).
    */

    for (i = text_pg1; i < text_pg1 + li.t_npg; i++) {
        int physical_frame = alloc_frame();
        if (physical_frame == ERROR) {
            close(fd);
            return KILL;
        } 
        map_page(proc->region1_pt, i, physical_frame, PROT_READ | PROT_WRITE);
    }


    /*
    * ==>> Then, data. Allocate "data_npg" physical pages and map them starting at
    * ==>> the  "data_pg1" in region 1 address space.
    * ==>> These pages should be marked valid, with a protection of
    * ==>> (PROT_READ | PROT_WRITE).
    */

    for (i = data_pg1; i < data_pg1 + data_npg; i++) {
        int physical_frame = alloc_frame();
        if (physical_frame == ERROR) {
            close(fd);
            return KILL;
        } 
        map_page(proc->region1_pt, i, physical_frame, PROT_READ | PROT_WRITE);
    }

    /* 
    * ==>> Then, stack. Allocate "stack_npg" physical pages and map them to the top
    * ==>> of the region 1 virtual address space.
    * ==>> These pages should be marked valid, with a
    * ==>> protection of (PROT_READ | PROT_WRITE).
    */

    for (i = MAX_PT_LEN - stack_npg; i < MAX_PT_LEN ; i++) {
        int physical_frame = alloc_frame();
        if (physical_frame == ERROR) {
            close(fd);
            return KILL;
        }   
        map_page(proc->region1_pt, i, physical_frame, PROT_READ | PROT_WRITE);
    }

    /*
    * ==>> (Finally, make sure that there are no stale region1 mappings left in the TLB!)
    */
    WriteRegister(REG_TLB_FLUSH, TLB_FLUSH_1);

    /*
    * All pages for the new address space are now in the page table.  
    */

    /*
    * Read the text from the file into memory.
    */
    lseek(fd, li.t_faddr, SEEK_SET);
    segment_size = li.t_npg << PAGESHIFT;
    if (read(fd, (void *) li.t_vaddr, segment_size) != segment_size) {
        close(fd);
        return KILL;   // see ykernel.h
    }

    /*
    * Read the data from the file into memory.
    */
    lseek(fd, li.id_faddr, 0);
    segment_size = li.id_npg << PAGESHIFT;

    if (read(fd, (void *) li.id_vaddr, segment_size) != segment_size) {
        close(fd);
        return KILL;
    }


    close(fd);			/* we've read it all now */


    /*
    * ==>> Above, you mapped the text pages as writable, so this code could write
    * ==>> the new text there.
    *
    * ==>> But now, you need to change the protections so that the machine can execute
    * ==>> the text.
    *
    * ==>> For each text page in region1, change the protection to (PROT_READ | PROT_EXEC).
    * ==>> If any of these page table entries is also in the TLB, 
    * ==>> you will need to flush the old mapping. 
    */

    for (i = text_pg1; i < text_pg1 + li.t_npg; i++) {
        //needs to go through virtual pages
        //get pfns
        //set the .prot at each pfn
        proc->region1_pt[i].prot = PROT_READ | PROT_EXEC;
        }

    WriteRegister(REG_TLB_FLUSH, TLB_FLUSH_1);


    /*
    * Zero out the uninitialized data area
    */
    bzero((void *)li.id_end, li.ud_end - li.id_end);

    /*
    * Set the entry point in the process's UserContext
    */

    /* 
    * ==>> (rewrite the line below to match your actual data structure) 
    * ==>> proc->uc.pc = (caddr_t) li.entry;
    */

    proc->user_context.pc = (caddr_t) li.entry;

    /*
    * Now, finally, build the argument list on the new stack.
    */


    memset(cpp, 0x00, VMEM_1_LIMIT - ((int) cpp));

    *cpp++ = (char *)argcount;		/* the first value at cpp is argc */
    cp2 = argbuf;
    for (i = 0; i < argcount; i++) {      /* copy each argument and set argv */
    *cpp++ = cp;
    strcpy(cp, cp2);
    cp += strlen(cp) + 1;
    cp2 += strlen(cp2) + 1;
    }
    free(argbuf);
    *cpp++ = NULL;			/* the last argv is a NULL pointer */
    *cpp++ = NULL;			/* a NULL pointer for an empty envp */

    return SUCCESS;
}

