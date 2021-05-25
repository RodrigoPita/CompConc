#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

//variaveis do problema
FILE *fd;                  //arquivo de log
int leitoras_ativas = 0;   //contador de threads lendo
int escritoras_ativas = 0; //contador de threads escrevendo
int escritoras_fila = 0;

//variaveis para sincronizacao
pthread_mutex_t mutex;
pthread_cond_t cond_leit, cond_escr;

void criarLog()
{
  fd = fopen("PadraoEscritorLeitor.py", "w+");
  fprintf(fd, "%s", "import teste\n");
  fprintf(fd, "%s", "le = teste.LE()\n");
  fclose(fd);
}

void escreverNoLog(const char * string, int id)
{
  fd = fopen("PadraoEscritorLeitor.py", "a+");
  fprintf(fd, string, id);
  fclose(fd);
}
//entrada leitura
void entrarLeitora(int id)
{
  pthread_mutex_lock(&mutex);
  //printf("L[%d] quer ler\n", id);
  while (escritoras_ativas > 0 || escritoras_fila > 0)
  {
    escreverNoLog("le.leitorBloqueado(%d)\n", id);
    if (escritoras_fila > 0)
    {
      pthread_cond_signal(&cond_escr);
    }
    pthread_cond_wait(&cond_leit, &mutex);
    //printf("L[%d] desbloqueou\n", id);
  }
  leitoras_ativas++;
  escreverNoLog("le.leitorLendo(%d)\n", id);
  pthread_mutex_unlock(&mutex);
}

//saida leitura
void sairLeitora(int id)
{
  pthread_mutex_lock(&mutex);
  escreverNoLog("le.leitorSaindo(%d)\n", id);
  leitoras_ativas--;
  if (leitoras_ativas == 0)
    pthread_cond_signal(&cond_escr);
  pthread_mutex_unlock(&mutex);
}

//entrada escrita
void entrarEscritora(int id)
{
  pthread_mutex_lock(&mutex);
  //printf("E[%d] quer escrever\n", id);
  while ((leitoras_ativas > 0) || (escritoras_ativas > 0))
  {
    escreverNoLog("le.escritorBloqueado(%d)\n", id);
    escritoras_fila++;
    pthread_cond_wait(&cond_escr, &mutex);
    escritoras_fila--;
    escreverNoLog("le.escritorDesbloqueado(%d)\n", id);
  }
  escritoras_ativas++;
  escreverNoLog("le.escritorEscrevendo(%d)\n", id);
  pthread_mutex_unlock(&mutex);
}

//saida escrita
void sairEscritora(int id)
{
  pthread_mutex_lock(&mutex);
  escreverNoLog("le.escritorSaindo(%d)\n", id);

  escritoras_ativas--;
  pthread_cond_signal(&cond_escr);
  pthread_cond_broadcast(&cond_leit);
  pthread_mutex_unlock(&mutex);
}

void prepararSincronizacao()
{
  criarLog();

  pthread_mutex_init(&mutex, NULL);
  pthread_cond_init(&cond_leit, NULL);
  pthread_cond_init(&cond_escr, NULL);
}
