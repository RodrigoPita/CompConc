#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

int L 4 //numero de threads leitoras
int E 2 //numero de threads escritoras

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
   printf("L[%d] quer ler\n", id);
   while(escritoras_ativas > 0 || escritoras_fila > 0) {
     printf("L[%d] bloqueou\n", id);
     pthread_cond_wait(&cond_leit, &mutex);
     printf("L[%d] desbloqueou\n", id);
   }
   leitoras_ativas++;
   pthread_mutex_unlock(&mutex);
}

//saida leitura
void sairLeitora (int id) {
   pthread_mutex_lock(&mutex);
   printf("L[%d] terminou de ler\n", id);
   leitoras_ativas--;
   if(leitoras_ativas==0) pthread_cond_signal(&cond_escr);
   pthread_mutex_unlock(&mutex);
}

//entrada escrita
void entrarEscritora (int id) {
   pthread_mutex_lock(&mutex);
   printf("E[%d] quer escrever\n", id);
   while((leitoras_ativas > 0) || (escritoras_ativas > 0)) {
     printf("E[%d] bloqueou\n", id);
     pthread_cond_wait(&cond_escr, &mutex);
     printf("E[%d] desbloqueou\n", id);
   }
   escritoras_ativas++;
   pthread_mutex_unlock(&mutex);
}

	//saida escrita
	void sairEscritora (int id) {
		pthread_mutex_lock(&mutex);
		printf("E[%d] terminou de escrever\n", id);
		escritoras_ativas--;
		if(escritoras_fila > 0){
			pthread_cond_signal(&cond_escr);
		}else{
			pthread_cond_broadcast(&cond_leit);
		}
		pthread_mutex_unlock(&mutex);
	}

//thread leitora
void * leitor (void * arg) {
  int *id = (int *) arg;
  while(1) {
    entrarLeitora(*id);
    printf("Leitora %d esta lendo\n", *id);
    sairLeitora(*id);
    sleep(1);
  } 
  free(arg);
  pthread_exit(NULL);
}

//thread leitora
void * escritor (void * arg) {
  int *id = (int *) arg;
  while(1) {
    entrarEscritora(*id);
    printf("Escritora %d esta escrevendo\n", *id);
    sairEscritora(*id);
    sleep(1);
  } 
  free(arg);
  pthread_exit(NULL);
}

