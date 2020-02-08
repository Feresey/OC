#include <malloc.h>
#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define ERROR_CREATE_THREAD -11
#define ERROR_JOIN_THREAD -12
#define SUCCESS 0
#define modsum(X, Y) (X + 1) % Y

typedef struct {
    double X;
    double* res;
} arg_t;

void* F(void* args)
{
    arg_t* arg = (arg_t*)args;
    *arg->res = cos(arg->X) + sin(arg->X);
    return SUCCESS;
}

int Parsing(char* str, double* left, double* right)
{
    int pos;
    if (!str || *str != '[')
        return 1;

    for (pos = 1; str[pos] == '.' || str[pos] == '-' || (str[pos] != ',' && str[pos] != '\0'); ++pos);

    if (str[pos] == '\0')
        return 1;
    else
        str[pos] = '\0';

    *left = atof(str + 1);
    *right = atof(str + pos + 1);
    return (*left < *right) ? 0 : 1;
}

int main(int argc, char** argv)
{
    pthread_t* thread;
    arg_t* arguments;
    int status, n;
    int* status_addr;
    double left, right;
    int threads_max = 5;
    double* res;
    char segment[sizeof(double) * 2 + 1];
    if (argc == 2) {
        threads_max = atoi(argv[1]);
    }

    printf("Max count of threads: %d\n", threads_max);
    printf("Number of points: ");
    scanf("%d", &n);
    printf("Segment: ");
    scanf("%s", segment);
    printf("\n");
    if (Parsing(segment, &left, &right))
        return 1;

    double eps = (right - left) / (n - 1);
    double iter = left;
    int num = 0;
    double prev = 1;

    thread = (pthread_t*)malloc(sizeof(pthread_t) * threads_max);
    arguments = (arg_t*)malloc(sizeof(arg_t) * threads_max);
    res = (double*)malloc(sizeof(double) * n);

    while (num < n) {
        int help_num = modsum(num, threads_max);

        arguments[help_num].X = iter;
        arguments[help_num].res = res + num;
        status = pthread_create(thread + help_num, NULL, &F, (void*)(arguments + help_num));
        if (status != SUCCESS) {
            printf("main error: can't create thread, status = %d\n", status);
            exit(ERROR_CREATE_THREAD);
        }
        iter += eps;
        help_num = modsum(help_num, threads_max);

        if (thread[help_num] != 0) {
            status = pthread_join(thread[help_num], (void**)&status_addr);
            if (status != SUCCESS) {
                fprintf(stdout, "main error: can't join thread, status = %d\n", status);
                exit(ERROR_JOIN_THREAD);
            }
            pthread_detach(thread[help_num]);
            thread[help_num] = 0;
        }
        num++;
    }

    for (int i = 0; i < threads_max; ++i) {
        if (thread[i] != 0) {
            status = pthread_join(thread[i], (void**)&status_addr);
            if (status != SUCCESS) {
                fprintf(stdout, "main error: can't join thread, status = %d\n", status);
                exit(ERROR_JOIN_THREAD);
            }
            pthread_detach(thread[i]);
        }
    }

    for (int i = 0; i < n; ++i) {
        prev *= res[i];
        printf("f(%.8lf) = %.16lf\n", left, prev);
        left += eps;
    }

    free(res);
    free(arguments);
    free(thread);
}