// Ryan Gonzalez, Sam Hirsh syscalls.h
// Header file for syscall dispatch and syscall handler
#ifndef SYSCALLS_H
#define SYSCALLS_H

#include <ykernel.h>



// syscall dispatcher
void syscall_dispatch(UserContext *uctxt);

// syscall handlers
int KernelFork(UserContext *uctxt);
int KernelExec(UserContext *uctxt);
int KernelExit(UserContext *uctxt);
int KernelWait(UserContext *uctxt);
int KernelGetPid(UserContext *uctxt);
int KernelBrk(UserContext *uctxt);
int KernelDelay(UserContext *uctxt);
int KernelTtyRead(UserContext *uctxt);
int KernelTtyWrite(UserContext *uctxt);
void syscall_handle(UserContext *uctxt);

#endif