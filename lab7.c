/* Disciplina: Computacao Concorrente */
/* Prof.: Silvana Rossetto */
/* Laboratório: 7 */
/* Codigo: Semáforos */

/***** Condicao logica da aplicacao: a thread D deve sempre imprimir sua mensagem antes das threads B e C,
a ordem em que as threads B e C imprimem suas mensagens nao importa, 
mas ambas devem sempre imprimir suas mensagens antes da thread A ****/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>

#define NTHREADS 4

/* Variaveis globais */
int x = 0; //variavel compartilhada entre as threads
sem_t condt1, condt2; //semaforos para sincronizar a ordem de execucao das threads

/* Thread A */
void *A (void *t) {
    int boba1, boba2;
    //printf("A: Comecei\n");
    /* faz alguma coisa pra gastar tempo... */
    boba1 = 100; boba2 = -100; while (boba2 < boba1) boba2++;
    sem_wait(&condt2);
    printf("Volte sempre!\n");
    
    pthread_exit(NULL);
}

/* Thread B */
void *B (void *t) {
    //printf("B: Comecei\n");
    sem_wait(&condt1);
    x++;
    printf("Fique a vontade.\n");
    if(x == 3)
        sem_post(&condt2);
    else
        sem_post(&condt1);
        
    pthread_exit(NULL);
}

/* Thread C */
void *C (void *t) {
    //printf("C: Comecei\n");
    sem_wait(&condt1);
    x++;
    printf("Sente-se por favor.\n");
    if(x == 3)
        sem_post(&condt2);
    else
        sem_post(&condt1);
        
    pthread_exit(NULL);
}

/* Thread D */
void *D (void *t) {
    //printf("D: Comecei\n");
    printf("Seja bem-vindo!\n");
    x++;
    sem_post(&condt1);

    pthread_exit(NULL);
}

/* Funcao principal */
int main(int argc, char *argv[]) {
    int i;
    pthread_t threads[NTHREADS];
    
    /* Inicia os semaforos */
    sem_init(&condt1, 0, 0);
    sem_init(&condt2, 0, 0);
    
    /* Cria as threads */
    if (pthread_create(&threads[0], NULL, A, NULL)) { printf("--ERRO: pthread_create()\n"); exit(-1); }
    if (pthread_create(&threads[1], NULL, B, NULL)) { printf("--ERRO: pthread_create()\n"); exit(-1); }
    if (pthread_create(&threads[2], NULL, C, NULL)) { printf("--ERRO: pthread_create()\n"); exit(-1); }
    if (pthread_create(&threads[3], NULL, D, NULL)) { printf("--ERRO: pthread_create()\n"); exit(-1); }
    
    /* Espera todas as threads completarem */
    for (i = 0; i < NTHREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    printf("\nFIM\n");
    
    /* Desaloca variaveis e termina */
    sem_destroy(&condt1);
    sem_destroy(&condt2);
}
