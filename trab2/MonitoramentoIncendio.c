#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <PadraoEscritorLeitor.h>


typedef struct{
	int temperatura;
	int idSensor;
	int idLeitura;
}medicao;

medicao buffer[60];
int leituras=0;
int in=0;
int out=0;

medicao bufferUltimasLeituras[15];
int ultimasLeituras=0;

//thread leitora
void * atuador (void * arg) {
	  int *id = (int *) arg;
	  while(1) {
	    entrarLeitora(*id);
	    printf("Leitora %d esta lendo\n", *id);
	    lerRegistro(int id);
	    sairLeitora(*id);
	    sleep(2);
	  } 
	  free(arg);
	  pthread_exit(NULL);
	}

void lerRegistro(int id){
	int somaTemperatura=0;
	for(int i=0; i<in; i++){
		medicao registro = buffer[i];
		if(registro.idSensor == id){
			bufferUltimasLeituras[ultimasLeituras] = registro;
			ultimasLeituras++;
			somaTemperatura += registro.temperatura;
		}
	}
	print("Media das temperaturas: %d", somaTemperatura/ultimasLeituras);
}

void verificarAlerta(){
	int temperaturaMaior35=0;
	int consecutividade=0;
	for(int i=0; i<ultimasLeituras;i++){
		int temperatura = bufferUltimasLeituras[ultimasLeituras].temperatura;
		if(temperatura > 35){
			temperaturaMaior35++;
			if(i == ultimasLeituras-6){
				consecutividade++;
			}
		}
	}

	if(temperaturaMaior35 == 5){
		if(consecutividade == 5){
			print("\nAlerta vermelho!");
		}else{
			print("\nAlerta amarelo!");
		}else{
			print("\nCondição normal.");
		}
	}
}

	//thread leitora
void * sensor (void * arg) {
	  int *id = (int *) arg;
	  int temperatura=0;
	  while(1) {
	    temperatura = medirTemperatura();
	    entrarEscritora(*id);
	    registrarTemperatura(temperatura, *id);
	    printf("Escritora %d esta escrevendo\n", *id);
	    sairEscritora(*id);
	    sleep(1);
	  } 
	  free(arg);
	  pthread_exit(NULL);
}

void registrarTemperatura(int temperatura, int id){
	medicao novo_registro;
	novo_registro.temperatura = temperatura;
	novo_registro.idSensor = id;
	novo_registro.idLeitura = leituras;

	buffer[leituras] = novo_registro;
	leituras++;
	in++;
}

int medirTemperatura(){
	int temperatura=0;
	while(temperatura < 31){
		sleep(1);
		temperatura = rand(%(40-25)+ 25);
	}
	return temperatura;
}


//funcao principal
int main(int argc, char *argv) {
	pthread_t tid[L+E];
	int id[L+E];

	if(argc < 3){
		printf("Digite %d <quantidade leitoras> <quantidade escritoras>", argv[0]);
		return 0;
  	}

	L = argv[1];
	E = argv[2];

	//inicializa as variaveis de sincronizacao
	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&cond_leit, NULL);
	pthread_cond_init(&cond_escr, NULL);

	//cria as threads leitoras
	for(int i=0; i<L; i++) {
		id[i] = i+1;
		if(pthread_create(&tid[i], NULL, leitor, (void *) &id[i])) exit(-1);
	} 
  
  //cria as threads escritoras
  for(int i=0; i<E; i++) {
    id[i+L] = i+1;
    if(pthread_create(&tid[i+L], NULL, escritor, (void *) &id[i+L])) exit(-1);
  } 

  pthread_exit(NULL);
  return 0;
}
