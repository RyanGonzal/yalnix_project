#include <yuser.h>

int main(void)
{
    while (1) {
        TracePrintf(0, "INIT RUNNING pid=%d\n", GetPid());
        Delay(5);
    }

    return 0;
}