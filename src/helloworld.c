#include <yuser.h>

int main(void)
{
    TracePrintf(1, "hello world started after Exec\n");

    while (1) {
        Delay(2);
    }
}