#include <stdio.h>     // printf
#include <string.h>    // memset
#include <sys/types.h> // for pid_t
#include <unistd.h>    // fork, read, write

char msg[] = "hello from parent process";
char buf[100];

int main(int argc, char **argv)
{
    int filedes[2];
    pid_t pid;

    //      Создадим пару файловых дескрипторов для пипы
    if (pipe(filedes) == -1)
    {
        perror("pipe");
        return 1;
    }

    //      Создадим процесс-потомок
    pid = fork();

    //      Оценим стуацию
    if (pid == -1)
    {
        perror("fork");
        return 1;
    }
    //      Родитель
    // printf("PID : %d\n",pid);
    if (pid)
    {
        // filedes[0] используется для чтения
        close(filedes[0]);
        // printf("Parent: sending message to child [ %d ]\n", pid);
        write(filedes[1], msg, sizeof(msg));
        // execlp(argv[1],argv[2]);
        // execl("hello", " ", "Hello", "World!", NULL);

        close(filedes[1]);
    }
    //      Потомок
    else
    {
        // filedes[1] используется для записи
        close(filedes[1]);
        printf("Выполняется %s", argv[0]);
        execv("/usr/bin/echo", argv);
        memset(buf, 0, sizeof(buf));
        read(filedes[0], buf, sizeof(msg));
        close(filedes[0]);
        // printf("Child: parent say: %s\n", buf);
    }

    return 0;
}