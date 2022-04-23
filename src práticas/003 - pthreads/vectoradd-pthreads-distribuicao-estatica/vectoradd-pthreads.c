#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// Entrada e saída.
float *h_a;
float *h_b;
float *h_c;

int partition = 0;
int n = 0;

void init_array(int n) {
  fprintf(stdout, "Inicializando os arrays.\n");
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
	fprintf(stdout, "h_c[%07d]: %f\n", i, h_c[i]);
  }
}

void check_result(int n){
  // Soma dos elementos do array C e divide por N, o valor deve ser igual a 1.
  int i;
  float sum = 0;
  fprintf(stdout, "Verificando o resultado.\n");  
  
  for (i = 0; i < n; i++) {
	  sum += h_c[i];
	}
	
	fprintf(stdout, "Resultado Final: (%f, %f)\n", sum, (float)(sum / (float)n));
}

void *vecadd(void *ptr) {
  int *id;
  int i, ii, ff;

  id = (int *) ptr;

  ii = (*id) * partition;
  ff = ii + partition; // ff = (ii + 1) * partition;

  printf("  Thread[%lu]: Particao prevista: %d [%d..%d]: %d.\n", (long int) pthread_self(), *id, ii, ff,  (ff - ii));

  /* A sobra n - ff sempre será maior que uma particao até a penúltima thread. Na última thread se sobrar algumas iterações
  no final, a última thread assume estendendo seu ff para n. */ 
  if((n - ff) < partition){
    ff = n;
  }

  printf("  Thread[%lu]: Executando sobre particao: %d [%d..%d]: %d.\n", (long int) pthread_self(), *id, ii, ff, (ff - ii));
  
  for (i = ii; i < ff; i++) {
    h_c[i] = h_a[i] + h_b[i];
  }

  printf("  Thread[%lu]: Terminando...\n", (long int) pthread_self());
  pthread_exit(0);
}

int main(int argc, char *argv[]) {
  int i, num_elementos, num_threads = 0;

  if(argc < 3){
    printf("Uso: %s <size> <num_threads>\n", argv[0]);
    exit(0);
  }

  num_elementos = atoi(argv[1]);
  num_threads = atoi(argv[2]);

  partition = num_elementos / num_threads;

  n = num_elementos;

  printf("num_elementos: %d num_threads: %d\n", num_elementos, num_threads);

  h_a = (float *) malloc(num_elementos * sizeof(float));
  h_b = (float *) malloc(num_elementos * sizeof(float));
  h_c = (float *) malloc(num_elementos * sizeof(float));

  init_array(num_elementos);

  pthread_t *threads;

  threads = (pthread_t *) malloc(num_threads * sizeof(pthread_t));

  int *irets;
  irets = (int *) malloc (num_threads * sizeof(int));

  int *ids;
  ids = (int *) malloc (num_threads * sizeof(int));

  for(i=0; i<num_threads; i++){
    ids[i] = i;
    irets[i] = pthread_create(&threads[i], NULL, vecadd, (void *)&ids[i]);
    printf("Thread[%lu]: Criacao da Thread %d com o id: %lu [%s]\n", (long int) pthread_self(), i, threads[i], ((irets[i] == 0)? "OK" : "Erro"));
  }
  
  for(i=0; i<num_threads; i++){
	  printf("Thread[%lu]: Aguardando o termino das threads...\n", (long int) pthread_self());
    pthread_join(threads[i], NULL);
  }

  printf("Thread[%lu]: Todas as threads terminaram...\n", (long int) pthread_self());
  
  printf("Thread[%lu]: Imprimindo o resultado.\n", (long int) pthread_self());
	
  printf("Thread[%lu]: Verificando o resultado.\n", (long int) pthread_self());
	check_result(num_elementos);

  printf("Thread[%lu]: Fui, Tchau!\n", (long int) pthread_self());
  
  return 0;
}
