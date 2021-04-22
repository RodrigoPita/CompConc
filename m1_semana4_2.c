/* Disciplina: Computacao Concorrente */
/* Prof .? Silbana Rossetto */
/* Codigo: Comunicação entre threads usando variável compartilhada */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NTHREADS 2

int s = 0; //variavel compartilhada entre as threads
pthread_mutex_t lock; //variavel especial para sincronizacao de exclusao mutua

//funcao executada pelas threads
void *ExecutaTarefa (void *arg) {
    long int id = (long int) arg;
    printf("Thread : %ld esta executando...\n", id);
    
    for (int i = 0; i < 10000; i++) {
        //entrada de secao critica
        pthread_mutex_lock(&lock);
        s++; //incrementa a variavel compartilhada
        //saida de secao critica
        pthread_mutex_unlock(&lock);
    }
    printf("Thread : %ld terminou!\n", id);
    pthread_exit(NULL);
}

//fluxo principal
int main(int argc, char *argv[]) {
    pthread_t tid[NTHREADS];
    
    //inicializa a variavel de exclusao mutua
    pthread_mutex_init(&lock, NULL);
    //--cria as threas
    for (long int t = 0; t < NTHREADS; t++) {
        if (pthread_create(&tid[t], NULL, ExecutaTarefa, (void *)t)) {
            printf("--ERRO: pthread_create()\n"); exit(-1);
        }
    }
    
    //--espera todas as threads terminarem
    for (int t = 0; t < NTHREADS; t++) {
        if (pthread_join(tid[t], NULL)) {
            printf("--ERRO: pthread_join()\n"); exit(-1);
        }
    }
    
    pthread_mutex_destroy(&lock);
    printf("Valor de s = %d\n", s);
    
    return 0;
}
