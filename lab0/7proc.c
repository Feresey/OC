#include <stdlib.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <errno.h>

int main()
{
    char pid[255];
    fork();
    fork();
    fork();
    sprintf(pid, "PID : %d\tPPID : %d\n", getpid(), getppid());
    write(STDOUT_FILENO, pid, strlen(pid));
    exit(0);
}
