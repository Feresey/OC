#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <malloc.h>
#include <semaphore.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

const int ARG_MAX = 2097152 + 1;

int main()
{
    pid_t pid;
    int rv, fd;
    char* memory;
    // создаем разделяемую память
    if ((fd = shm_open("shared_file", O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR)) == -1) {
        perror("shm::open_fail");
        exit(-1);
    }

    // задаем ей нужный размер
    if (ftruncate(fd, ARG_MAX * 2) == -1) {
        perror("trucate::fail");
        exit(-1);
    }

    memory = mmap(NULL, ARG_MAX * 2, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0); // отображаем на вирт. память

    if (memory == MAP_FAILED) {
        perror("mmap::mapping_fail");
        fprintf(stderr, "%p", memory);
        exit(-1);
    }
    close(fd);

    char* command = memory;
    char* filename = memory + ARG_MAX;

    *command = *filename = '\0';

    switch (pid = fork()) {
    case -1: {
        perror("fork");
        exit(1);
    }

    case 0: {
        // Потомок
        while (*command == '\0' || *filename == '\0')
            sleep(1);
        FILE* my = freopen(filename, "r", stdin);
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

    default: {
        //Родитель
        scanf("%s %s", command, filename);
        waitpid(pid, &rv, 0);
        exit(WEXITSTATUS(rv));
    }
    }
    if (munmap(memory, ARG_MAX * 2)) {
        perror("munmap");
        exit(-1);
    }
}
