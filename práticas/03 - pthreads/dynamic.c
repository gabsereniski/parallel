#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <inttypes.h>
#include <stdbool.h>
#include "timer.h"

double *a, *b, *c;
int partition = 0;
int last_assigned = 0;
int total_iterations = 0;
bool work_finished = false;

sem_t mutex;

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

bool get_next_partition(long *ii, long *ff)
{
    bool ret = true;
    *ii = *ff = 0;

    sem_wait(&mutex);

    if(work_finished) ret = false;
    
    else
    {
        *ii = last_assigned;
        *ff = last_assigned + partition;
        if(*ff > total_iterations) *ff = total_iterations;

        last_assigned = *ff;
        ret = true;
        
        work_finished = (last_assigned == total_iterations);        
    }

    sem_post(&mutex);

    return ret;
}

void *vector_add(void *ptr)
{
    long *id = (long*)ptr;
    long ii, ff;
    bool have_work = false;

    while(have_work = get_next_partition(&ii, &ff))
    {
        for(int i = ii; i < ff; i++)
            c[i] = a[i] + b[i];
    }

    pthread_exit(0);
}

int main(int argc, char **argv)
{
    if(argc < 4)
    {
        printf("%s <size> <num_threads> < partition_size>\n", argv[0]);
        exit(0);
    }

    int num_elements = atoi(argv[1]);
    int num_threads = atoi(argv[2]);
    partition = atoi(argv[3]);

    total_iterations = num_elements;

    printf("num_elements: %d, num_threads: %d\n\n", num_elements, num_threads);

    malloc_arrays(num_elements);
    init_array(num_elements);

    pthread_t *threads = (pthread_t*)malloc(num_threads * sizeof(pthread_t));
    int *irets = (int*)malloc(num_threads * sizeof(int));
    long *ids = (long*)malloc(num_threads * sizeof(long));

    sem_init(&mutex, 0, 1);

    work_finished = false;

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