#include <errno.h>
#include <malloc.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int Parsing(char* str, double* left, double* right)
{
    int pos;
    if (!str)
        return 1;
    if (*str != '[')
        return 1;

    for (pos = 1; str[pos] == '.'
     || (str[pos] != ',' && str[pos] != '\0'); ++pos)
        ;

    if (str[pos] == '\0')
        return 1;
    else
        str[pos] = '\0';

    *left = atof(str + 1);
    *right = atof(str + pos + 1);
    return (*left < *right) ? 0 : 1;
}

double f(int X, double prev)
{
    return (sin(X) + cos(X)) * prev;
}

int main(int argc, char const* argv[])
{
    pid_t pid;
    int n;
    double left, right;
    int threads = 0;
    double* res;
    char segment[sizeof(double) * 2 + 1];
    if (argc == 2) {
        threads = atoi(argv[1]);
    }

    printf("Max count of threads: %d\n", threads);
    printf("Number of points: ");
    scanf("%d", &n);
    printf("Segment: ");
    scanf("%s", segment);

    res = (double*)malloc(sizeof(double) * n);
    if (Parsing(segment, &left, &right))
        return 1;
    printf("l: %f, r: %f\n", left, right);
    double eps = (right - left) / n;
    double iter = left;
    int num = 0;
    while (iter + eps <= right) {
        res[num] = f(iter, res[num]);
        ++num;
        iter += eps;
        //     if (threads) {
        //         pid = fork();
        //         switch (pid) {
        //         case 0: {
        //             res[num] = f(iter, res, num);
        //             break;
        //         }
        //         }
    }

    for (int i = 0; i < n; i++) {
        printf("f(%f) = %f\n", left, res[i]);
        left += eps;
    }
    free(res);
    return 0;
}
