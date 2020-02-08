#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>

int main()
{
    pid_t p;

    printf("Original program, \t pid=%d\n", getpid());
    p = fork();
    if (p == 0)
    {
        printf("In child process, \t pid=%d, \t ppid=%d\n",
               getpid(), getppid());
    }
    else
    {
        printf("In parent, \t\t pid=%d, \t fork returned=%d\n",
               getpid(), p);
    }
}
