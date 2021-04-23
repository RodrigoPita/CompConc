/* Disciplina: Computacao Concorrente */
/* Prof.: Silvana Rossetto */
/* Laboratório: 4 */
/* Codigo: Atividade 4 */

/***** Condicao logica da aplicacao: a thread D deve sempre imprimir sua mensagem antes das threads B e C,
a ordem em que as threads B e C imprimem suas mensagens nao importa, 
mas ambas devem sempre imprimir suas mensagens antes da thread A ****/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define NTHREADS 4

/* Variaveis globais */
int x = 0;
pthread_mutex_t x_mutex;
pthread_cond_t x_cond1, x_cond2;

/* Thread A */
void *A (void *t) {
    int boba1, boba2;
    
    //printf("A: Comecei\n");
    
    /* faz alguma coisa pra gastar tempo... */
    boba1 = 100; boba2 = -100; while (boba2 < boba1) boba2++;
    
    pthread_mutex_lock(&x_mutex);
    if (x < 3) {
        //printf("A: x = %d, vai se bloquear...\n", x);
        pthread_cond_wait(&x_cond2, &x_mutex);
        //printf("A: sinal recebido e mutex realocado, x = %d\n", x);
    }
    printf("Volte sempre!\n");
    pthread_mutex_unlock(&x_mutex);
    
    pthread_exit(NULL);
}

/* Thread B */
void *B (void *t) {
    //printf("B: Comecei\n");
    
    pthread_mutex_lock(&x_mutex);
    if(x < 1) {
        //printf("B: x = %d, vai se bloquear...\n", x);
        pthread_cond_wait(&x_cond1, &x_mutex);
        //printf("B: sinal recebido e mutex realocado, x = %d\n", x);
    }
    x++;
    printf("Fique a vontade.\n");
    if(x == 3)
        pthread_cond_signal(&x_cond2);
    else
        pthread_cond_signal(&x_cond1);
    pthread_mutex_unlock(&x_mutex);
    pthread_exit(NULL);
}

/* Thread C */
void *C (void *t) {
    //printf("C: Comecei\n");
    
    pthread_mutex_lock(&x_mutex);
    if(x < 1) {
        //printf("C: x = %d, vai se bloquear...\n", x);
        pthread_cond_wait(&x_cond1, &x_mutex);
        //printf("B: sinal recebido e mutex realocado, x = %d\n", x);
    }
    x++;
    printf("Sente-se por favor.\n");
    if(x == 3)
        pthread_cond_signal(&x_cond2);
    else
        pthread_cond_signal(&x_cond1);
    pthread_mutex_unlock(&x_mutex);
    pthread_exit(NULL);
}

/* Thread D */
void *D (void *t) {
    //printf("D: Comecei\n");
    
    pthread_mutex_lock(&x_mutex);
    printf("Seja bem-vindo!\n");
    x++;
    pthread_cond_signal(&x_cond1);
    pthread_mutex_unlock(&x_mutex);
    
    pthread_exit(NULL);
}

/* Funcao principal */
int main(int argc, char *argv[]) {
    int i;
    pthread_t threads[NTHREADS];
    
    /* Inicializa o mutex (lock de exclusao mutua) e a variavel de condicao */
    pthread_mutex_init(&x_mutex, NULL);
    pthread_cond_init(&x_cond1, NULL);
    pthread_cond_init(&x_cond2, NULL);
    
    /* Cria as threads */
    pthread_create(&threads[0], NULL, A, NULL);
    pthread_create(&threads[1], NULL, B, NULL);
    pthread_create(&threads[2], NULL, C, NULL);
    pthread_create(&threads[3], NULL, D, NULL);
    
    /* Espera todas as threads completarem */
    for (i = 0; i < NTHREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    printf("\nFIM\n");
    
    /* Desaloca variaveis e termina */
    pthread_mutex_destroy(&x_mutex);
    pthread_cond_destroy(&x_cond1);
    pthread_cond_destroy(&x_cond2);
}
