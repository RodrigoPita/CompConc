#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

//variaveis do problema
int leitoras_ativas=0; //contador de threads lendo
int escritoras_ativas=0; //contador de threads escrevendo
int escritoras_fila=0;

//variaveis para sincronizacao
pthread_mutex_t mutex;
pthread_cond_t cond_leit, cond_escr;

//entrada leitura
void entrarLeitora(int id) {
   pthread_mutex_lock(&mutex);
  //printf("L[%d] quer ler\n", id);
   while(escritoras_ativas > 0 || escritoras_fila > 0) {
     //printf("L[%d] bloqueou\n", id);
     if(escritoras_fila > 0){
       pthread_cond_signal(&cond_escr);
     }
     pthread_cond_wait(&cond_leit, &mutex);
     //printf("L[%d] desbloqueou\n", id);
   }
   leitoras_ativas++;
   pthread_mutex_unlock(&mutex);
}

//saida leitura
void sairLeitora (int id) {
   pthread_mutex_lock(&mutex);
   //printf("L[%d] terminou de ler\n", id);
   leitoras_ativas--;
   if(leitoras_ativas==0) pthread_cond_signal(&cond_escr);
   pthread_mutex_unlock(&mutex);
}

//entrada escrita
void entrarEscritora (int id) {
   pthread_mutex_lock(&mutex);
   //printf("E[%d] quer escrever\n", id);
   while((leitoras_ativas > 0) || (escritoras_ativas > 0)) {
     //printf("E[%d] bloqueou\n", id);
     escritoras_fila++;
     pthread_cond_wait(&cond_escr, &mutex);
     escritoras_fila--;
     //printf("E[%d] desbloqueou\n", id);
   }
   escritoras_ativas++;
   pthread_mutex_unlock(&mutex);
}

//saida escrita
void sairEscritora (int id) {
  pthread_mutex_lock(&mutex);
  //printf("E[%d] terminou de escrever\n", id);
  escritoras_ativas--;
  pthread_cond_signal(&cond_escr);
  pthread_cond_broadcast(&cond_leit);
  pthread_mutex_unlock(&mutex);
}

void prepararSincronizacao(){
  pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&cond_leit, NULL);
	pthread_cond_init(&cond_escr, NULL);
}

