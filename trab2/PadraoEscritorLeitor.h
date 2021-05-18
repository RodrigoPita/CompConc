#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define int L=0 //numero de threads leitoras
#define int E=0 //numero de threads escritoras

#define int leitoras_ativas=0; //contador de threads lendo
#define int escritoras_ativas=0; //contador de threads escrevendo
#define int escritoras_fila=0;

#define pthread_mutex_t mutex;
#define pthread_cond_t cond_leit, cond_escr;

//entrada leitura
#define void entrarLeitora(int id);

//saida leitura
#define void sairLeitora (int id) ;

//entrada escrita
#define void entrarEscritora (int id) ;

//saida escrita
#define void sairEscritora (int id);

