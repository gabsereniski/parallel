#include <stdio.h>
#include <stdlib.h>
#include "timer.h"

double *a, *b, *c;

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

void vector_add(int n)
{
    for(int i = 0; i < n; i++)
        c[i] = a[i] + b[i];
}

int main(int argc, char **argv)
{
    if(argc < 2)
    {
        printf("%s <size>\n", argv[0]);
        exit(0);
    }

    int n = atoi(argv[1]);

    printf("num_elements: %d\n", n);

    malloc_arrays(n);
    init_array(n);

    Timer *t = new_timer();

    start_timer(t);
    vector_add(n);
    stop_timer(t);

    printf("Checking result: ");
    check_result(n);

    printf("\nExecution time: %.5f\n", get_timer_result(t));

    delete_timer(&t);
    free_arrays();

    return 0;
}