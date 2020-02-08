#include <malloc.h>
#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <semaphore.h>
#include <stdlib.h>

#define ERROR_CREATE_THREAD -11
#define ERROR_JOIN_THREAD -12
#define SUCCESS 0
#define modsum(X) (X + 1) % threads_max

sem_t sema;

int Parsing(char* str, double* left, double* right);

typedef struct {
    double X;
    double res;
} arg_t;

void* F(void* args)
{
    arg_t* arg = (arg_t*)args;
    arg->res = cos(arg->X) + sin(arg->X);
    return SUCCESS;
}


int main(int argc, char** argv)
{
    pthread_t* thread;
    arg_t* arguments;
    int num = 0,
        status = 0,
        n = 0,
        **status_addr = NULL,
        help_num = 0,
        help_num_2 = 0,
        threads_max = 5;
    double prev = 1,
           left = 0,
           right = 0;
    char segment[600];

    if (argc == 2)
        threads_max = atoi(argv[1]);
    if (threads_max <= 0) {
        printf("Wrong thread count!\n");
        exit(1);
    }

    printf("Max count of threads: %d\n", threads_max);
    printf("Number of points: ");
    scanf("%d", &n);
    printf("Segment: ");
    scanf("%s", segment);
    printf("\n");

    if (Parsing(segment, &left, &right))
        exit(1);

    thread = (pthread_t*)malloc(sizeof(pthread_t) * threads_max);
    arguments = (arg_t*)malloc(sizeof(arg_t) * threads_max);
    for (int i = 0; i < threads_max; ++i)
        thread[i] = 0;

    double eps = (right - left) / (n - 1);
    double iter = left;

    while (num < n) {
        help_num = modsum(help_num);
        arguments[help_num].X = iter;
        status = pthread_create(thread + help_num, NULL, F, (void*)(arguments + help_num));
        if (status != SUCCESS) {
            printf("main error: can't create thread, status = %d\n", status);
            exit(ERROR_CREATE_THREAD);
        }

        help_num_2 = modsum(help_num);
        if (thread[help_num_2]) {
            status = pthread_join(thread[help_num_2], (void**)status_addr);
            if (status != SUCCESS) {
                fprintf(stdout, "main error: can't join thread, status = %d\n", status);
                exit(ERROR_JOIN_THREAD);
            }
            thread[help_num_2] = 0;
            prev *= arguments[help_num_2].res;
            printf("f(%2.10f) = %2.20f\n", arguments[help_num_2].X, prev);
        }
        iter += eps;
        num++;
    }

    for (help_num = 0; help_num < threads_max && thread[help_num]; ++help_num)
        ;

    for (int i = help_num, num = 0; num < threads_max; ++num) {
        if (thread[i]) {
            status = pthread_join(thread[i], (void**)status_addr);
            if (status != SUCCESS) {
                fprintf(stdout, "main error: can't join thread, status = %d\n", status);
                exit(ERROR_JOIN_THREAD);
            }
            prev *= arguments[i].res;
            printf("f(%2.10f) = %2.20f\n", arguments[i].X, prev);
        }
        i = modsum(i);
    }

    free(arguments);
    free(thread);
}

int Parsing(char* str, double* left, double* right)
{
    int pos;
    if (!str || *str != '[')
        return 1;

    for (pos = 1; str[pos] == '.' || str[pos] == '-' || (str[pos] != ',' && str[pos] != '\0'); ++pos)
        ;

    if (str[pos] == '\0')
        return 1;
    else
        str[pos] = '\0';

    *left = atof(str + 1);
    *right = atof(str + pos + 1);
    return (*left < *right) ? 0 : 1;
}
