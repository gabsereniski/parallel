#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <inttypes.h>
#include <stdbool.h>

// Entrada e saída.
float *h_a;
float *h_b;
float *h_c;

int partition = 0;
int last_assigned = 0;
int total_of_iterations = 0;
bool work_finished = false;

sem_t mutex;

void init_array(int n) {
  fprintf(stdout, "Thread[%lu]: Initializing the arrays.\n", (long int) pthread_self());
  int i;
  // Initialize vectors on host.
	for (i = 0; i < n; i++) {
	  h_a[i] = 0.5;
	  h_b[i] = 0.5;
	}
}

void print_array(int n) {
  int i;
  for (i = 0; i < n; i++) {
    fprintf(stdout, "Thread[%lu]: h_c[%07d]: %f.\n", (long int) pthread_self(), i, h_c[i]);
  }
}

void check_result(int n){
  // Soma dos elementos do array C e divide por N, o valor deve ser igual a 1.
  int i;
  float sum = 0;
  fprintf(stdout, "Thread[%lu]: Checking.\n", (long int) pthread_self());
  
  for (i = 0; i < n; i++) {
	  sum += h_c[i];
	}
	
  fprintf(stdout, "Thread[%lu]: Final Result: (%f, %f).\n", (long int) pthread_self(), sum, (float)(sum / (float)n));
}

bool get_next_loop_partition(long *ii, long *ff) {
  bool ret = true;

  *ii = 0;
  *ff = 0;

  fprintf(stdout, "Thread[%lu]: Trying to get the next partition.\n", (long int) pthread_self());

  sem_wait(&mutex);
  
  if(work_finished){
    ret = false;
  }
  else{
    *ii = last_assigned;
    *ff = last_assigned + partition;

    if (*ff > total_of_iterations){
      *ff = total_of_iterations;
    }

    last_assigned = *ff;
    ret = true;

    work_finished = (last_assigned == total_of_iterations);
  }

  sem_post(&mutex);

  fprintf(stdout, "Thread[%lu]: Got the partition [%lu, %lu].\n", (long int) pthread_self(), *ii, *ff);

  return ret;
}

void *vecadd(void *ptr) {
  long *id;
  long i, ii, ff;

  id = (long *) ptr;
  bool have_work = false;

  while ((have_work = get_next_loop_partition(&ii, &ff)) != false){
    fprintf(stdout, "   Thread[%lu,%lu]: Working on partition: [%d..%d]: %d.\n", *id, (long int) pthread_self(), ii, ff, (ff - ii));
    for (i = ii; i < ff; i++) {
      h_c[i] = h_a[i] + h_b[i];
    }  
  }
  if(!have_work){
    fprintf(stdout, "  Thread[%lu]: No work to do.\n", (long int) pthread_self());
  }
  fprintf(stdout, "  Thread[%lu]: Exiting.\n", (long int) pthread_self());
  pthread_exit(0);
}

int main(int argc, char *argv[]) {
  int i, num_elements, num_threads = 0;

  if(argc < 4){
    fprintf(stderr, "Uso: %s <num_elements> <num_threads> <partition_size>\n", argv[0]);
    exit(0);
  }

  num_elements = atoi(argv[1]);
  num_threads = atoi(argv[2]);
  partition = atoi(argv[3]);

  total_of_iterations = num_elements;

  fprintf(stdout, "Thread[%lu]: num_elements: %d num_threads: %d.\n", (long int) pthread_self(), num_elements, num_threads);

  fprintf(stdout, "Thread[%lu]: Allocating the arrays.\n", (long int) pthread_self());
  h_a = (float *) malloc(num_elements * sizeof(float));
  h_b = (float *) malloc(num_elements * sizeof(float));
  h_c = (float *) malloc(num_elements * sizeof(float));

  init_array(num_elements);

  pthread_t *threads;

  threads = (pthread_t *) malloc(num_threads * sizeof(pthread_t));

  int *irets;
  irets = (int *) malloc (num_threads * sizeof(int));

  long *ids;
  ids = (long *) malloc (num_threads * sizeof(long));

  sem_init(&mutex, 0, 1);      /* initialize mutex to 1 - binary semaphore */
                                 /* second param = 0 - semaphore is local */

  work_finished = false;

  fprintf(stdout, "Thread[%lu]: Creating the Threads.\n", (long int) pthread_self());

  for(i=0; i<num_threads; i++){
    ids[i] = i;
    irets[i] = pthread_create(&threads[i], NULL, vecadd, (void *)&ids[i]);
    fprintf(stdout, "Thread[%lu]: Create Thread %d with id: %lu [%s]\n", (long int) pthread_self(), i, threads[i], ((irets[i] == 0)? "OK" : "Error"));
  }
  
  for(i=0; i<num_threads; i++){
	  fprintf(stdout, "Thread[%lu]: Waiting for ending of threads execution.\n", (long int) pthread_self());
    pthread_join(threads[i], NULL);
  }

  fprintf(stdout, "Thread[%lu]: All threads were finished.\n", (long int) pthread_self());
  
  fprintf(stdout, "Thread[%lu]: Printing the result.\n", (long int) pthread_self());
	
  fprintf(stdout, "Thread[%lu]: Checking the result.\n", (long int) pthread_self());
	check_result(num_elements);

  fprintf(stdout, "Thread[%lu]: Fui, Tchau!\n", (long int) pthread_self());
  
  return 0;
}
