
// Ryan Gonzalez, Sam Hirsh traps.h
// Header file for trap setup and trap handler
#ifndef TRAPS_H
#define TRAPS_H

#include <ykernel.h>


// trap setup
void traps_init(void);

// trap handlers(missing other traps I think not sure if in context for checkpoint 1)
// missing tty, arithmetic, 
void trap_kernel(UserContext *uctxt);
void trap_clock(UserContext *uctxt);
void trap_memory(UserContext *uctxt);
void trap_unhandled(UserContext *uctxt);
void syscall_handle(UserContext *uctxt);

#endif