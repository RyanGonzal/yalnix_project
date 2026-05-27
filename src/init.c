#include <yuser.h>

int main(void)
{
    //init test
    // while (1) {
    //     TracePrintf(0, "INIT RUNNING pid=%d\n", GetPid());
    //     Delay(5);
    // }

    // return 0;
    //  int pid = Fork();

    //fork test 
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
    //wait and exit 
    // int pid;
    // int status;
    // int waited;

    // pid = Fork();

    // if (pid == 0) {
    //     TracePrintf(1, "child: before Exit\n");
    //     Exit(7);
    //     TracePrintf(1, "child: after Exit SHOULD NOT PRINT\n");
    // }

    // TracePrintf(1, "parent: forked child=%d\n", pid);

    // waited = Wait(&status);

    // TracePrintf(1, "parent: Wait returned pid=%d status=%d\n",
    //             waited, status);

    // while (1) {
    //     Delay(2);
    // }
    //exec test
    // char *args[] = {"init", NULL};

    // TracePrintf(1, "init before Exec\n");

    // Exec("init", args);

    // TracePrintf(1, "init after Exec SHOULD ONLY PRINT IF EXEC FAILED\n");

    // while (1) {
    //     Delay(2);
    // }
    // exec and hello world test
    // char *args[] = {"helloworld", NULL};

    // TracePrintf(1, "init before Exec hello\n");

    // Exec("helloworld", args);

    // TracePrintf(1, "init after Exec SHOULD NOT PRINT\n");

    // while (1) {
    //     Delay(2);
    // }
    int pid;

    pid = Fork();

    if (pid == 0) {
        TracePrintf(1, "child running pid=%d\n", GetPid());

        while (1) {
            Delay(2);
        }
    }

    TracePrintf(1, "parent running pid=%d child=%d\n", GetPid(), pid);

    while (1) {
        Delay(2);
    }
}