#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

char buf[1024];

int main(void)
{
    int l;
    pid_t pid;
    int sig1[2], sig2[2];

    if (pipe(sig1))
    {
        perror("pipe1");
        return 1;
    }
    if (pipe(sig2))
    {
        perror("pipe2");
        return 2;
    }

    switch (pid = fork())
    {
    case -1:
        perror("fork");
        return 3;
    case 0: /*Child*/
        close(sig1[0]);
        close(sig2[1]);
        strcpy(buf, "Hello, papa!");
        l = strlen(buf);

        write(sig1[1], &l, sizeof(int));
        write(sig1[1], buf, l + 1);

        read(sig2[0], &l, sizeof(int));
        read(sig2[0], buf, l);

        printf("Child from the father: '%s'\n", buf);
        return 0;
    default: /*The father*/
        close(sig1[1]);
        close(sig2[0]);
        break;
    }

    read(sig1[0], &l, sizeof(int));
    read(sig1[0], buf, l);
    printf("Father from child: '%s'\n", buf);

    strcpy(buf, "Hello, baby!");
    l = strlen(buf);

    write(sig2[1], &l, sizeof(int));
    write(sig2[1], buf, l + 1);

    wait(&l);
    return 0;
}
