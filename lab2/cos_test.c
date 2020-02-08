#include <math.h>
#include <stdio.h>
#include <time.h>

int main()
{
    double iter = 0.5;
    double eps = 1;
    clock_t tm;
    // time(&tm);
    // printf("%li\n", clock() - tm);
    // clock();
    while (1 + eps / 2 < eps + 1)
        eps /= 2;

    for (int i = 0; i < 1000; i++) {
        tm = clock();
        cos(iter);
        printf("%li\n", clock() - tm);

        // printf("%li\n", tm) - time(&tme);
    }
    return 0;
}
