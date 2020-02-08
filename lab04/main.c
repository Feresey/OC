#include <ctype.h>
#include <fcntl.h>
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

#define FILE_NAME "shared_memory"
#define MAX_EXPR_L 1000000 // max length of expression
#define SIZE MAX_EXPR_L + 2 * sizeof(sem_t) // extra memory for 2 semaphores

int main()
{
    int cheak_error;
    int fd = shm_open(FILE_NAME, 
    O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
     // ~создаем разделяемую память~
    if (fd == -1) {
        perror("shm::open_fail");
        exit(-1);
    }
    cheak_error = ftruncate(fd, SIZE);
    if (cheak_error == -1) { //~ задаем ей нужный размер (для разделяемой памяти так можно, для обычных файлов -- не всегда. Для низ лучше lseek, write)~
        perror("trucate::fail");
        exit(-1);
    }
    char* memory = mmap(NULL, SIZE,
     PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
     // ~отображаем на вирт. память~
    if (memory == MAP_FAILED) {
        perror("mmap::mapping_fail");
        fprintf(stderr, "%p", memory);
        exit(-1);
    }
    close(fd); // ~дескриптор больше не нужен~

    int shift = 0;

    sem_t* ready_to_comp = (sem_t*)(memory + shift);
    shift += sizeof(sem_t);
    sem_init(ready_to_comp, 1, 0);

    char* buffer = (char*)(memory + shift);

    buffer[0] = '\0';
    pid_t process;
    int number_process;
    printf("~Введите желаемое число процессов~\n");
    scanf("%d", &number_process);
    for (int i = 0; i < number_process; i++) {
        process = fork();
        if (process == -1) {
            perror("fork");
            exit(1);
        } else if (process == 0) {
            sem_wait(ready_to_comp);
            char* file_name_out = (char*)malloc(sizeof(char) * 50);
            sscanf(buffer, "%s", file_name_out);
            FILE* out_file;
            out_file = fopen(file_name_out, "a");
            if (out_file) {
                char sing = ' ';
                int shift1 = 0;
                while (1) {
                    sscanf(buffer + strlen(file_name_out) + shift1, 
                    "%c", &sing);
                    if (sing == '0') {
                        exit(0);
                    }
                    fprintf(out_file, "%c", sing);
                    shift1++;
                }
            }
            fclose(out_file);
            return 0;
        }

        else {

            printf("~Введите желаемое имя файла~\n");
            char* file_name = (char*)malloc(sizeof(char) * 50);
            scanf("%s", file_name);
            sprintf(buffer, "%s", file_name);
            char sing_input = ' ';
            int j = 0;
            while (1) {
                scanf("%c", &sing_input);
                sprintf(buffer + strlen(file_name) + j, "%c", sing_input);
                if (sing_input == '0') {
                    break;
                }
                j++;
            }
            sem_post(ready_to_comp);
        }
    }
    if (munmap(memory, SIZE)) {
        perror("mmap");
        exit(-1);
    }
    sem_destroy(ready_to_comp);
    return 0;
}
