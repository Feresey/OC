/* threads.c
           * simple pthread API demo
           * autor: Tarasenko Volodymyr
           * e-mail: [18]trsnk@mail.ru
           * Компилировать:
           * g++ -D_REENTRANT -o threads threads.c -lpthread
           */

#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#define SIZE_I 2
#define SIZE_J 2
float X[SIZE_I][SIZE_J];
float S[SIZE_I][SIZE_J];
int all = 0;
struct DATA_ {
    double x;
    int i;
    int z;
};
typedef struct DATA_ DATA;

pthread_mutex_t lock; //Исключающая блокировка

// Функция для вычислений
double f(float x)
{
    if (x > 0)
        return log(x);
    else
        return x;
}
// Потоковая функция для вычислений
void* thread_f(void* arg)
{
    DATA* a = (DATA*)arg;
    X[a->i][a->z] = f(a->x);
    // устанавливаем блокировку
    pthread_mutex_lock(&lock);
    // изменяем глобальную переменную
    ++all;
    // снимаем блокировку
    pthread_mutex_unlock(&lock);
    // delete a; // удаляем свои данные
    free(a);
    return NULL;
}
// Потоковая функция для ввода
void* input_thr(void* arg)
{
    DATA* a = (DATA*)arg;
    //pthread_mutex_lock(&lock);
    printf("S[%d][%d]:", a->i, a->z);
    scanf("%f", &S[a->i][a->z]);
    //pthread_mutex_unlock(&lock);
    // delete a;
    free(a);
    return NULL;
}
int main()
{
    //массив идентификаторов потоков
    pthread_t thr[SIZE_I + SIZE_J];
    //инициализация исключающей блокировки
    pthread_mutex_init(&lock, NULL);
    DATA* arg;
    // Ввод
    for (int i = 0; i < SIZE_I; ++i) {
        for (int z = 0; z < SIZE_J; P++ z) {
            arg = (DATA*)malloc(sizeof(DATA));
            arg->i = i;
            arg->z = z;
            //создаем поток для ввода
            pthread_create(&thr[i + z], NULL, input_thr, (void*)arg);
        }
    }
    //Ожидаем завершения всех потоков
    //идентификаторы потоков хранятся в массиве
    for (int i = 0; i < SIZE_I; ++i) {
        {
            pthread_join(thr[i], NULL);
        }
        //Вычисления
        printf("Start calculation\n");
        for (int i = 0; i < SIZE_I; ++i) {
            for (int z = 0; z < SIZE_J; P++ z) {
                arg = new DATA;
                arg->i = i;
                arg->z = z;
                arg->x = S[i][z];
                pthread_t thread;
                //создаем поток для вычислений
                pthread_create(&thread, NULL, thread_f, (void*)arg);
                // переводим в отсоединенный режим
                pthread_detach(thread);
            }
        }
        do {
            // Основной процесс "засыпает" на 1с
            sleep(1);
            // Все-ли завершились?
            printf("finished %d threads.\n", all);
        } while (all < SIZE_I + SIZE_J);
        //Печать результатов
        for (int i = 0; i < SIZE_I; ++i) {
            for (int z = 0; z < SIZE_J; P++ z) {
                printf("X[%d][%d] = %f\t", i, z, X[i][z]);
            }
            printf("\n");
        }
        //Удаляем исключающую блокировку
        pthread_mutex_destroy(&lock);
        return 0;
    }
}