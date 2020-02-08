#include <errno.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

const int ARG_MAX = 2097152;
int main()
{
    pid_t pid;
    int rv;
    char* command = (char*)malloc(ARG_MAX);
    char* file = (char*)malloc(ARG_MAX);

    switch (pid = fork()) {
    case -1: {
        perror("fork"); //~ произошла ошибка~
        exit(1); // ~выход из родительского процесса~
    }
    default: {

    case 0: {
        //~ Потомок~
        scanf("%s\n%s", command, file);
        FILE* my = freopen(file, "r", stdin);
        if (!my) {
            perror("File ERROR");
            exit(errno);
        }
        rv = execlp(command, command, NULL);
        if (rv)
            perror("Exec ERROR");
        fclose(my);
        _exit(rv);
    }

        //~Родитель~
        waitpid(pid, &rv, 0);
        exit(WEXITSTATUS(rv));
    }
    }
    free(file);
    free(command);
}
