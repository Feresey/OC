#include <malloc.h>
#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define ERROR_CREATE_THREAD -11
#define ERROR_JOIN_THREAD -12
#define SUCCESS 0

int Parsing(char* str, double* left, double* right)
{
    int pos;
    if (!str)
        return 1;
    if (*str != '[')
        return 1;

    for (pos = 1; str[pos] == '.' || (str[pos] != ',' && str[pos] != '\0'); ++pos)
        ;

    if (str[pos] == '\0')
        return 1;
    else
        str[pos] = '\0';

    *left = atof(str + 1);
    *right = atof(str + pos + 1);
    return (*left < *right) ? 0 : 1;
}

typedef struct _arg {
    double X;
    double* res;
} arg_t;

double f(double X)
{
    return cos(X) + sin(X);
}

void* F(void* args)
{
    arg_t* arg = (arg_t*)args;
    // pthread_detach(pthread_self());
    *arg->res = cos(arg->X) + sin(arg->X);
    // pthread_exit(NULL);
    return SUCCESS;
}

// void* helloWorld(void* args)
// {
//     printf("Hello from thread!\n");
//     return SUCCESS;
// }

int main(int argc, char** argv)
{
    pthread_t* thread;
    int status;
    int* status_addr;
    int n;
    double left, right;
    int threads_max = 8;
    double* res;
    char segment[sizeof(double) * 2 + 1];
    if (argc == 2) {
        threads_max = atoi(argv[1]);
    }

    printf("Max count of threads: %d\n", threads_max);
    printf("Number of points: ");
    scanf("%d", &n);

    thread = (pthread_t*)malloc(sizeof(pthread_t) * n);

    printf("Segment: ");
    scanf("%s", segment);
    printf("\n");
    res = (double*)malloc(sizeof(double) * n);
    if (Parsing(segment, &left, &right))
        return 1;

    double eps = (right - left) / n;
    double iter = left;
    int num = 0;
    arg_t* arguments = (arg_t*)malloc(sizeof(arg_t) * n);

    while (num < n) {
        arguments[num].X = iter;
        arguments[num].res = res + num;
        status = pthread_create(thread + num, NULL, &F, (void*)(arguments + num));
        if (status != SUCCESS) {
            printf("main error: can't create thread, status = %d\n", status);
            exit(ERROR_CREATE_THREAD);
        }
        ++num;
        iter += eps;
        if (num > threads_max) {
            status = pthread_join(thread[num - threads_max], (void**)&status_addr);
            if (status != SUCCESS) {
                printf("main error: can't join thread, status = %d\n", status);
                exit(ERROR_JOIN_THREAD);
            }
            pthread_detach(thread[num - threads_max]);
        }
        // else if (num +threads_max >=n)
        // {
        //     status = pthread_join(thread[num], (void**)&status_addr);
        //     if (status != SUCCESS) {
        //         printf("main error: can't join thread, status = %d\n", status);
        //         exit(ERROR_JOIN_THREAD);
        //     }
        //     pthread_detach(thread[num]);
        //     perror("lol");
        // }
    }
    // for (int i = n - 1; n - 1 - i < threads_max; --i) {
    //     status = pthread_join(thread[num], (void**)&status_addr);
    //     if (status != SUCCESS) {
    //         printf("main error: can't join thread, status = %d\n", status);
    //         exit(ERROR_JOIN_THREAD);
    //     }
    //     // pthread_detach(thread[i]);
    // }

    for (int i = 0; i < n; ++i) {
        printf("f(%.8lf) = %.16lf\n", left, res[i]);
        left += eps;
    }
    free(res);
    free(arguments);
    free(thread);
}

// int mai()
// {
//     pthread_t thread;
//     int status;
//     int status_addr;

//     status = pthread_create(&thread, NULL, helloWorld, NULL);
//     if (status != 0) {
//         printf("main error: can't create thread, status = %d\n", status);
//         exit(ERROR_CREATE_THREAD);
//     }
//     printf("Hello from main!\n");

//     status = pthread_join(thread, (void**)&status_addr);
//     if (status != SUCCESS) {
//         printf("main error: can't join thread, status = %d\n", status);
//         exit(ERROR_JOIN_THREAD);
//     }

//     printf("joined with address %d\n", status_addr);
//     // _getch();
//     return 0;
// }