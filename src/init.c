#include <yuser.h>

int main(void)
{
    // while (1) {
    //     TracePrintf(0, "INIT RUNNING pid=%d\n", GetPid());
    //     Delay(5);
    // }

    // return 0;
     int pid = Fork();

    if (pid == 0) {
        TracePrintf(0, "child: pid=%d\n", GetPid());
        Exit(42);
    } else {
        TracePrintf(0, "parent: pid=%d child=%d\n", GetPid(), pid);
    }

    while (1) {
        Delay(2);
    }
}