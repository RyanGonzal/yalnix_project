#include <yuser.h>

int main(void)
{
    // while (1) {
    //     TracePrintf(0, "INIT RUNNING pid=%d\n", GetPid());
    //     Delay(5);
    // }

    // return 0;
    //  int pid = Fork();

    // if (pid == 0) {
    //     TracePrintf(0, "child: pid=%d\n", GetPid());
    //     Exit(42);
    // } else {
    //     TracePrintf(0, "parent: pid=%d child=%d\n", GetPid(), pid);
    // }

    // while (1) {
    //     Delay(2);
    // }
    // int pid;

    // pid = Fork();

    // if (pid == 0) {
    //     TracePrintf(1, "child before Exit\n");
    //     Exit(7);
    //     TracePrintf(1, "child after Exit SHOULD NOT PRINT\n");
    // }

    // TracePrintf(1, "parent after Fork child=%d\n", pid);

    // while (1) {
    //     Delay(2);
    // }
    int pid;
    int status;
    int waited;

    pid = Fork();

    if (pid == 0) {
        TracePrintf(1, "child: before Exit\n");
        Exit(7);
        TracePrintf(1, "child: after Exit SHOULD NOT PRINT\n");
    }

    TracePrintf(1, "parent: forked child=%d\n", pid);

    waited = Wait(&status);

    TracePrintf(1, "parent: Wait returned pid=%d status=%d\n",
                waited, status);

    while (1) {
        Delay(2);
    }
}