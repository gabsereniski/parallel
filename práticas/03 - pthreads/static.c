#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "timer.h"

double *a, *b, *c;
int partition = 0;
int n = 0;

void malloc_arrays(int n)
{
    a = (double*)malloc(n * sizeof(double));
    b = (double*)malloc(n * sizeof(double));
    c = (double*)malloc(n * sizeof(double));
}

void free_arrays()
{
    free(a), free(b), free(c);
}

void init_array(int n)
{
    for(int i = 0; i < n; i++)
        a[i] = b[i] = 0.5;
}

void print_array(int n)
{
    for(int i = 0; i < n; i+=1000)
        printf("c[%012d]: %f\n", i, c[i]);
}

void check_result(int n)
{
    double sum = 0.0;

    for(int i = 0; i < n; i++)
        sum += c[i];
    
    printf("(%f, %f)\n", sum, sum/(double)n); 
}


void *vector_add(void *ptr)
{
    int *id = (int*)ptr;
    int ii, ff;

    ii = (*id) * partition;
    ff = ((n - ff) < partition) ? n : ii + partition;

    for(int i = ii; i < ff; i++)
        c[i] = a[i] + b[i];
    
    pthread_exit(0);
}

int main(int argc, char **argv)
{
    if(argc < 3)
    {
        printf("%s <size> <num_threads>\n", argv[0]);
        exit(0);
    }

    int num_elements = atoi(argv[1]);
    int num_threads = atoi(argv[2]);

    partition = num_elements/num_threads;
    n = num_elements;

    printf("num_elements: %d, num_threads: %d\n", num_elements, num_threads);

    malloc_arrays(n);
    init_array(n);

    pthread_t *threads = (pthread_t*)malloc(num_threads * sizeof(pthread_t));
    int *irets = (int*)malloc(num_threads * sizeof(int));
    int *ids = (int*)malloc(num_threads * sizeof(int));

    Timer *t = new_timer();

    start_timer(t);

    for(int i = 0; i < num_threads; i++)
    {
        ids[i] = i;
        irets[i] = pthread_create(&threads[i], NULL, vector_add, (void*)&ids[i]);
    }

    for(int i = 0; i < num_threads; i++)
        pthread_join(threads[i], NULL);

    stop_timer(t);
    
    printf("Checking result: ");
    check_result(num_elements);

    printf("\nExecution time: %.5f\n", get_timer_result(t));

    delete_timer(&t);
    free_arrays();

    return 0;
}