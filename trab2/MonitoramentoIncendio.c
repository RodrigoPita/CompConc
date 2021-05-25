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

#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_BLUE "\x1b[34m"
#define ANSI_COLOR_RESET "\x1b[0m"

#define TAM_BUFFER 60

FILE *fdApp;
medicao buffer[TAM_BUFFER];
int escritas = 0;
int leituras = 0;
pthread_mutex_t mutexLog;

void criarLogApp()
{
	fdApp = fopen("MonitoramentoIncendio.py", "w+");
	fprintf(fdApp, "%s", "import teste\n");
	fprintf(fdApp, "%s", "le = teste.LE()\n");
	fclose(fdApp);
}

void escreverNoLogAplicacao(const char *string, int temp, int elementos, int alerta)
{
	pthread_mutex_lock(&mutexLog);
	fdApp = fopen("MonitoramentoIncendio.py", "a+");
	fprintf(fdApp, string, temp, elementos, alerta);
	fclose(fdApp);
	pthread_mutex_unlock(&mutexLog);
}

void escreverNoLogAplicacaoNormal(const char *string, int temp, int alerta)
{
	pthread_mutex_lock(&mutexLog);
	fdApp = fopen("MonitoramentoIncendio.py", "a+");
	fprintf(fdApp, string, temp, alerta);
	fclose(fdApp);
	pthread_mutex_unlock(&mutexLog);
}

void verificarAlerta(int idAtuador)
{
	int temperaturasMaior35 = 0;
	int alertaVermelho = 0;
	int elementosSensor = 0;
	int i;
	int k;

	//percorre inicio do vetor partindo da última escrita
	for (i = (escritas % TAM_BUFFER); i > 0; i--)
	{
		medicao registro = buffer[i];
		int temperatura = registro.temperatura;
		int idSensor = registro.idSensor;
		if (idSensor == idAtuador)
		{
			elementosSensor++; //quantidade de registro encontradas para o sensor
			if (temperatura > 35)
			{
				temperaturasMaior35++;
				if (elementosSensor == temperaturasMaior35)
				{
					printf(ANSI_COLOR_RED "Leitura %d: idSensor[%d] temperatura[%d] idLeitura[%d]\n" ANSI_COLOR_RESET, 15 - elementosSensor, registro.idSensor, temperatura, registro.idLeitura);
				}
				else
				{
					printf(ANSI_COLOR_YELLOW "Leitura %d: idSensor[%d] temperatura[%d] idLeitura[%d]\n" ANSI_COLOR_RESET, 15 - elementosSensor, registro.idSensor, temperatura, registro.idLeitura);
				}
			}
			else
			{
				printf("Leitura %d: idSensor[%d] temperatura[%d] idLeitura[%d]\n", 15 - elementosSensor, registro.idSensor, temperatura, registro.idLeitura);
			}
			if (elementosSensor == 5 && temperaturasMaior35 == 5)
				alertaVermelho = 1;
		}
		if (elementosSensor == 15)
			break; //encerra ao encontrar 15 registros
	}

	//percorre final do vetor em casos da escrita já começar a sobreescrever o início
	if (elementosSensor < 15 && escritas > TAM_BUFFER)
	{
		for (k = TAM_BUFFER; k > (escritas % TAM_BUFFER); k--)
		{
			medicao registro = buffer[k];
			int temperatura = registro.temperatura;
			int idSensor = registro.idSensor;
			if (idSensor == idAtuador)
			{
				elementosSensor++;
				if (temperatura > 35)
				{
					temperaturasMaior35++;
					if (elementosSensor == temperaturasMaior35)
					{
						printf(ANSI_COLOR_RED "Leituras %d: idSensor[%d] temperatura[%d] idLeitura[%d]\n" ANSI_COLOR_RESET, 15 - elementosSensor, registro.idSensor, temperatura, registro.idLeitura);
					}
					else
					{
						printf(ANSI_COLOR_YELLOW "Leituras %d: idSensor[%d] temperatura[%d] idLeitura[%d]\n" ANSI_COLOR_RESET, 15 - elementosSensor, registro.idSensor, temperatura, registro.idLeitura);
					}
				}
				else
				{
					printf("Leituras %d: idSensor[%d] temperatura[%d] idLeitura[%d] \n", 15 - elementosSensor, registro.idSensor, temperatura, registro.idLeitura);
				}
				if (elementosSensor == 5 && temperaturasMaior35 == 5)
					alertaVermelho = 1;
			}
			if (elementosSensor == 15)
				break; //encerra ao encontrar 15 registros
		}
	}

	if (temperaturasMaior35 >= 5)
	{
		if (alertaVermelho)
		{
			printf("Sensor %d: Alerta vermelho!\n\n", idAtuador);
			escreverNoLogAplicacao("le.disparandoAlertaVermelho(%d, %d, %d)\n", temperaturasMaior35, elementosSensor, alertaVermelho);
		}
		else
		{
			printf("Sensor %d: Alerta amarelo!\n\n", idAtuador);
			escreverNoLogAplicacao("le.disparandoAlertaAmarelo(%d, %d, %d)\n", temperaturasMaior35, elementosSensor, alertaVermelho);
		}
	}
	else
	{
		printf("Sensor %d: Condição normal.\n\n", idAtuador);
		escreverNoLogAplicacaoNormal("le.disparandoCondicaoNormal(%d, %d)\n", temperaturasMaior35, alertaVermelho);
	}
}

void calcularMediaTemperatura(int idAtuador)
{
	int somaTemperatura = 0;
	int mediaTemperatura = 0;
	int temperaturasSensor = 0;
	for (int i = 0; i < TAM_BUFFER; i++)
	{
		if (buffer[i].idSensor == idAtuador)
		{
			somaTemperatura += buffer[i].temperatura;
			temperaturasSensor++;
		}
	}

	if (temperaturasSensor > 0)
	{
		mediaTemperatura = somaTemperatura / temperaturasSensor;
		printf("\n\nMedia das temperaturas do sensor %d: %d\n", idAtuador, mediaTemperatura);
	}
}

void lerRegistro(int idAtuador)
{
	calcularMediaTemperatura(idAtuador);

	if (escritas >= 5)
	{
		verificarAlerta(idAtuador);
	}
}

void registrarTemperatura(int temperatura, int id)
{
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
		if (escritas > 0)
		{ //incia leitura apenas quando houver escrita
			entrarLeitora(*id);
			lerRegistro(*id);
			sairLeitora(*id);
		}
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

	//cria ou limpa arquivo de log
	criarLogApp();

	//inicializa as variaveis de sincronizacao
	prepararSincronizacao();

	//cria as threads leitoras
	for (int i = 0; i < L; i++)
	{
		id[i] = i + 1;
		if (pthread_create(&tid[i], NULL, atuador, (void *)&id[i]))
			exit(-1);
		id[i + E] = i + 1;
		if (pthread_create(&tid[i], NULL, sensor, (void *)&id[i + E]))
			exit(-1);
	}

	pthread_exit(NULL);
	return 0;
}
