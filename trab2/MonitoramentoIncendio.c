#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "PadraoEscritorLeitor.h"

typedef struct
{
	int temperatura;
	int idSensor;
	int idLeitura;
} medicao;

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_YELLOW   "\x1b[33m"
#define ANSI_COLOR_BLUE   "\x1b[34m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define TAM_BUFFER 60

pthread_mutex_t mutexLeituras;

medicao buffer[TAM_BUFFER];
int escritas = 0;
int leituras = 0;


void verificarAlerta()
{
	int temperaturaMaior35 = 0;
	int consecutividade = 0;
	int elementosNoFinalBuffer = 0;
	int i;
	int k =0;
	
	i = (leituras % TAM_BUFFER) - 15;//recebe a posicao inicial das 15 ultimas leituras
	if(i < 0){//caso negativo, pode significar que o buffer atingiu o seu limite e agora está sobreescrevendo os antigos, ou que ainda temos menos que 15 escritas
		if(leituras >= TAM_BUFFER){
			elementosNoFinalBuffer = (-1) * i;
			k = TAM_BUFFER - elementosNoFinalBuffer;
		} //caso haja igual ou mais leituras que o tamanho do buffer, significa que de fato está ocorrendo sobreescrita, 
		//então k recebe a posição necessária para percorrer o extremo que está ao final do vetor
		
		i = 0;//nesse caso, a posição inicial será na extremidade que corresponde ao inicio do vetor
	}

	//percorre final caso necessário
	if(k>0){
		for (k; k < TAM_BUFFER; k++)
		{
			medicao registro = buffer[k];
			int temperatura = registro.temperatura;
			if (temperatura > 35)
			{
				temperaturaMaior35++;
				if(elementosNoFinalBuffer > 10 && k == TAM_BUFFER - 1){
					consecutividade++;
					printf(ANSI_COLOR_RED "Leituras: idSensor[%d] temperatura[%d] idLeitura[%d]\n" ANSI_COLOR_RESET, registro.idSensor, temperatura, registro.idLeitura);
				}else{
					printf(ANSI_COLOR_YELLOW "Leituras: idSensor[%d] temperatura[%d] idLeitura[%d]\n" ANSI_COLOR_RESET, registro.idSensor, temperatura, registro.idLeitura);	
				}
			}else{
				printf("Leituras: idSensor[%d] temperatura[%d] idLeitura[%d] \n", registro.idSensor, temperatura, registro.idLeitura);
			}
		}
	}

	//percorre inicio do vetor
	for (i; i < (leituras % TAM_BUFFER); i++)
	{
		medicao registro = buffer[i];
		int temperatura = registro.temperatura;
		if (temperatura > 35)
		{
			temperaturaMaior35++;
			if (i > (leituras % TAM_BUFFER - 6))
			{
				consecutividade++;
				printf(ANSI_COLOR_RED "Leituras: idSensor[%d] temperatura[%d] idLeitura[%d]\n" ANSI_COLOR_RESET, registro.idSensor, temperatura, registro.idLeitura);
			}else{
				printf(ANSI_COLOR_YELLOW "Leituras: idSensor[%d] temperatura[%d] idLeitura[%d]\n" ANSI_COLOR_RESET, registro.idSensor, temperatura, registro.idLeitura);	
			}
		}else{
			printf("Leituras: idSensor[%d] temperatura[%d] idLeitura[%d]\n" , registro.idSensor, temperatura, registro.idLeitura);	
		}
	}

	if (temperaturaMaior35 > 5)
	{
		if (consecutividade == 5)
		{
			printf("Alerta vermelho!\n\n");
		}
		else
		{
			printf("Alerta amarelo!\n\n");
		}
	}
	else
	{
		printf("Condição normal.\n\n");
	}

}

void calcularMediaTemperatura(int idAtuador){
	int somaTemperatura = 0;
	int mediaTemperatura=0;
	int temperaturasSensor=0;
	for(int i = 0; i < (escritas % TAM_BUFFER); i++){
		if (buffer[i].idSensor == idAtuador)
		{
			somaTemperatura += buffer[i].temperatura;
			temperaturasSensor++;
		}
	}

	if(temperaturasSensor > 0){
		mediaTemperatura = somaTemperatura / temperaturasSensor;
		printf("Media das temperaturas do sensor %d: %d\n",idAtuador, mediaTemperatura);
	}
}

void lerRegistro(int idAtuador)
{
	if(escritas > 0){
		//printf("Atuador %d esta lendo, há %d\n", id, leituras);
		pthread_mutex_lock(&mutexLeituras);
		
		medicao registro = buffer[leituras % TAM_BUFFER];
		if (registro.idSensor == idAtuador)
		{
			leituras++;
			if(leituras >= 5){
				verificarAlerta();
			}
			calcularMediaTemperatura(idAtuador);
		}
		
		pthread_mutex_unlock(&mutexLeituras);
	}
}

void registrarTemperatura(int temperatura, int id)
{
	//printf("Sensor %d esta escrevendo\n", id);
	medicao novo_registro;
	novo_registro.temperatura = temperatura;
	novo_registro.idSensor = id;
	novo_registro.idLeitura = escritas;

	buffer[escritas % TAM_BUFFER] = novo_registro;
	escritas++;
}

int medirTemperatura()
{
	int temperatura = 0;
	while (temperatura < 31)
	{
		sleep(1);
		temperatura = (rand() % 41) + 25;
	}
	return temperatura;
}

//thread leitora
void *atuador(void *arg)
{
	int *id = (int *)arg;
	while (1)
	{
		entrarLeitora(*id);
		lerRegistro(*id);
		sairLeitora(*id);
		sleep(2);
	}
	free(arg);
	pthread_exit(NULL);
}


//thread escritora
void *sensor(void *arg)
{
	int *id = (int *)arg;
	int temperatura = 0;
	while (1)
	{
		temperatura = medirTemperatura();
		entrarEscritora(*id);
		registrarTemperatura(temperatura, *id);
		sairEscritora(*id);
	}
	free(arg);
	pthread_exit(NULL);
}

//funcao principal
int main(int argc, char *argv[])
{
	int L, E;
	pthread_t tid[100];
	int id[100];

	if (argc < 2)
	{
		printf("Digite %s <quantidade de sensores>\n", argv[0]);
		return 0;
	}

	L = atoi(argv[1]); //devem possuir mesmo id e portanto mesma quantidade
	E = atoi(argv[1]);

	//inicializa as variaveis de sincronizacao
	prepararSincronizacao();
	pthread_mutex_init(&mutexLeituras, NULL);

	//cria as threads leitoras
	for (int i = 0; i < L; i++)
	{
		id[i] = i + 1;
		if (pthread_create(&tid[i], NULL, atuador, (void *)&id[i]))
			exit(-1);
		id[i+E] = i + 1;
		if (pthread_create(&tid[i], NULL, sensor, (void *)&id[i+E]))
			exit(-1);
	}

	pthread_exit(NULL);
	return 0;
}
