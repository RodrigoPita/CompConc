#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include "timer.h"

long long int dim; //dimensao do vetor de entrada
int nthreads; //numero de nthreads
double *vetor; //vetor de entrada com dimensao dim

//fluxo das threads
void * tarefa(void * arg) {
    long int id = (long int) arg; //identificador da thread
    double *somaLocal; //variavel local da soma de elementos
    long int tamBloco = dim/nthreads; //tamanho do bloco de cada thread
    long int ini = id * tamBloco; //elemento inicial do bloco da thread
    long int fim; //elemento final(nao processado) do bloco da thread
    
    somaLocal = (double*) malloc(sizeof(double));
    if(somaLocal == NULL) {
        fprintf(stderr, "ERRO-malloc\n"); exit(1);
    }
    *somaLocal = 0;
    if(id == nthreads-1) fim = dim;
    else fim = ini + tamBloco; //trata o resto se houver
    //soma os elementos do bloco da thread
    for(long int i = ini; i < fim; i++)
        *somaLocal += vetor[i];
    
    //retorna o resultado da soma local
    pthread_exit((void *) somaLocal);
}

//fluxo principal
int main(int argc, char *argv[]) {
    double somaSeq = 0; //soma sequencial
    double somaConc = 0; //soma concorrente
    double ini, fim; //tomada de tempo
    pthread_t *tid;
    double *retorno; //valor de retorno das threads
    //recebe os parametros de entrada (dimensao do vetor, numero d ethreads)
    if(argc < 3) {
        fprintf(stderr, "Digite: %s ,dimensao do vetor> <numero de threads>\n", argv[0]);
        return 1;
    }
    dim = atoll(argv[1]);
    nthreads = atoi(argv[2]);
    //aloca o vetor de entrada
    vetor = (double*) malloc(sizeof(double)*dim);
    if(vetor == NULL) {
        fprintf(stderr, "ERRO--malloc\n");
        return 2;
    }
    //preenche o vetor de entrada
    GET_TIME(ini);
    for(long int i = 0, j = 0; i < dim; i++, j += 2) {
        if(i % 2 == 0) {
            vetor[i] = 1.0/(j+1);
        }
        else {
            vetor[i] = -1.0/(j+1);
        }
    }
    //soma sequencial dos elementos
    for(long int i = 0; i < dim; i++)
        somaSeq += vetor[i];
    GET_TIME(fim);
    printf("Tempo sequencial: %lf\n", fim-ini);
    //soma concorrente dos elementos
    GET_TIME(ini);
    tid = (pthread_t *) malloc(sizeof(pthread_t) * nthreads);
    if(tid == NULL) {
        fprintf(stderr, "ERRO--malloc\n");
        return 2;
    }
    //criar as threads
    for(long int i = 0; i < nthreads; i++) {
        if(pthread_create(tid + i, NULL, tarefa, (void*) i)) {
            fprintf(stderr, "ERRO--pthread_create\n");
            return 3;
        }
    }
    //aguardar o termino das threads
    for(long int i = 0; i < nthreads; i++) {
        if(pthread_join(*(tid + i), (void**) &retorno)) {
            fprintf(stderr, "ERRO--pthread_create\n");
            return 3;
        }
        //soma global
        somaConc += *retorno;
        free(retorno);
    }
    GET_TIME(fim);
    printf("Tempo concorrente: %lf\n", fim-ini);
    //exibir os resultados
    printf("Soma seq: %.15lf\n", 4*somaSeq);
    printf("Soma conc: %.15lf\n", 4*somaConc);
    printf("Pi: %.15lf\n", M_PI);
    printf("Diferenças entre Pi e as somas encontradas\nSeq: %.15lf\nConc: %.15lf\n", M_PI-(4*somaSeq), M_PI-(4*somaConc));
    
    //libera as areads de memoria alocadas
    free(vetor);
    free(tid);
}
