/* Disciplina: Computacao Concorrente */
/* Aluno.: Rodrigo Pita */
/* Módulo 1 - Laboratório: 1 */
/* Codigo: Atividade 5 */

#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h>

#define NTHREADS  2 //total de threads a serem criadas
#define NELEMENTS 10000
#define HALFNELEMENTS 5000

int vet[NELEMENTS] = {};

//cria a estrutura de dados para armazenar os argumentos da thread
typedef struct {
   int idThread, nThreads, eleVetor[NELEMENTS];
} t_Args;

//funcao executada pelas threads
void * tarefa (void *arg) {
  t_Args *args = (t_Args *) arg;
  
  int idThread = args->idThread;
  if (idThread > 0) {
    
    //incrementa 1 aos elementos do vetor
    for(int i = 0; i < HALFNELEMENTS; i++)
    vet[i]++;
    //args->eleVetor[i]++;
  } else {
    //incrementa 1 aos elementos do vetor de elementos
    for(int i = HALFNELEMENTS; i < NELEMENTS; i++)
    vet[i]++;
    //args->eleVetor[i]++;
    
  }
  printf("Ola, sou a thread %d e incrementei 1 em todos os elementos do vetor!\n", args->idThread+1);
  
  //imprime o vetor de elementos
  for(int i = 0; i < NELEMENTS; i++)
    printf("%d ", vet[i]);
    //printf("%d ", args->eleVetor[i]);
  printf("\n");
  
  free(arg); //aqui pode liberar a alocacao feita na main
  pthread_exit(NULL);
}


//funcao principal do programa
int main() {
  //int vetor[NELEMENTS] = {0};
  pthread_t tid_sistema[NTHREADS]; //identificadores das threads no sistema
  int thread; //variavel auxiliar
  t_Args *arg; //receberá os argumentos para a thread

  for(thread=0; thread<NTHREADS; thread++) {
    printf("--Aloca e preenche argumentos para thread %d\n", thread);
    arg = malloc(sizeof(t_Args));
    if (arg == NULL) {
      printf("--ERRO: malloc()\n"); exit(-1);
    }
    arg->idThread = thread; 
    arg->nThreads = NTHREADS;
    //arg->eleVetor[NELEMENTS] = vetor[NELEMENTS];
    
    printf("--Cria a thread %d\n", thread+1);
    if (pthread_create(&tid_sistema[thread], NULL, tarefa, (void*) arg)) {
      printf("--ERRO: pthread_create()\n"); exit(-1);
    }
  }

  //--espera todas as threads terminarem
  for (thread=0; thread<NTHREADS; thread++) {
    if (pthread_join(tid_sistema[thread], NULL)) {
         printf("--ERRO: pthread_join() \n"); exit(-1); 
    } 
  }
  //imprime o vetor de elementos
  printf("\n\n");
  for(int i = 0; i < NELEMENTS; i++)
    printf("%d ", vet[i]);
    //printf("%d ", arg->eleVetor[i]);
  printf("\n");

  printf("--Thread principal terminou\n");
  pthread_exit(NULL);
}
