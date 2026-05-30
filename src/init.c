#include <yuser.h>

int main(void)
{
    /*
     * TEST 1: Basic init + Delay
     * Expected:
     *   init repeatedly prints its pid, then sleeps with Delay.
     */
    // while (1) {
    //     TracePrintf(0, "INIT RUNNING pid=%d\n", GetPid());
    //     Delay(5);
    // }
    // return 0;


    /*
     * TEST 2: Basic Fork
     * Expected:
     * parent prints its pid and child pid.
     * child should print its own pid.
     * BUGGY
     */

    // int pid;
    
    // pid = Fork();
    
    // if (pid == 0) {
    //     TracePrintf(0, "child: pid=%d\n", GetPid());
    // } else {
    //     TracePrintf(0, "parent: pid=%d child=%d\n", GetPid(), pid);
    // }
    
    // while (1) {
    //     Delay(2);
    // }


    /*
     * TEST 3: Fork + Exit
     * Expected:
     * child exits with status 7.
     * "child after Exit SHOULD NOT PRINT" should never print.
     */
    // int pid;
    //
    // pid = Fork();
    //
    // if (pid == 0) {
    //     TracePrintf(1, "child before Exit\n");
    //     Exit(7);
    //     TracePrintf(1, "child after Exit SHOULD NOT PRINT\n");
    // }
    //
    // TracePrintf(1, "parent after Fork child=%d\n", pid);
    //
    // while (1) {
    //     Delay(2);
    // }


    /*
     * TEST 4: Fork + Wait + Exit
     * Expected:
     * parent waits for child.
     * Wait returns child pid and child's exit status.
     */
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


    /*
     * TEST 5: Exec into init
     * Expected
     * init reloads itself.
     * "init after Exec SHOULD ONLY PRINT IF EXEC FAILED" should not print
     * if Exec succeeds.
     */
    // char *args[] = {"init", NULL};
    //
    // TracePrintf(1, "init before Exec\n");
    //
    // Exec("init", args);
    //
    // TracePrintf(1, "init after Exec SHOULD ONLY PRINT IF EXEC FAILED\n");
    //
    // while (1) {
    //     Delay(2);
    // }


    /*
     * TEST 6: Exec into helloworld
     * Expected:
     * init execs into helloworld.
     * helloworld should print its startup message.
     * "init after Exec SHOULD NOT PRINT" should not print if Exec succeeds.
     */
    // char *args[] = {"helloworld", NULL};
    //
    // TracePrintf(1, "init before Exec hello\n");
    //
    // Exec("helloworld", args);
    //
    // TracePrintf(1, "init after Exec SHOULD NOT PRINT\n");
    //
    // while (1) {
    //     Delay(2);
    // }


    TtyWrite(0, "hello from tty\n", 15);

    while (1) {
        Delay(2);
   
    }
}
