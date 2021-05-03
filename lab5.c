/* Disciplina: Computacao Concorrente */
/* Prof.: Silvana Rossetto */
/* Laboratório 5 */
/* Codigo:  Problema de soma de prefixo (prefix sum)
            Entrada: um vetor de inteiros com 2^n elementos
            Saida: somas parciais em cada posicao do vetor 
            (somatorio de todos os elementos que antecedem essas posicoes, 
            incluindo o elemento da propria posicao*/

#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>

//variaveis globais
int leitr = 0; //contador de threads leitoras
int escr = 0; //numero de threads escritoras
int sinal = 1; //verificador se o numero eh positivo ou negativo
int *entrVet; //vetor de entrada
int *testVet; //vetor de teste
int vetElem = 0; //numero de elementos no vetor
int vetTam = 2; //tamanho do vetor
int nThreadsOn = 0; //numero de threads que continuam para a proxima fase

pthread_mutex_t mutex; //variavel para lock de secao critica
pthread_cond_t cond_prox; //variavel condicional para a secao critica

//alocacao de memoria
int* alocVet(int entrVetTam){
    int *vet;
    vet = (int *) malloc(sizeof(int)*entrVetTam);
    if(vet == NULL) {printf("--ERRO malloc\n");exit(-1);}
    return vet;
}

int* alocVetNovo(int *vet, int entrVetTam){
    int *vetNovo;
    vetNovo = alocVet(entrVetTam);
    for(int i = 0; i < entrVetTam; i++){
        *(vetNovo + i) = *(vet + i);
    }
    return vetNovo;
}

//adiciona as entradas como int ao vetor
void addEntrVet(int *entrVet, char entrada){
    int entrNum;
    entrNum = entrada - '0';
    *(entrVet + vetElem) = entrNum * sinal;
    *(testVet + vetElem) = entrNum * sinal;
    if(vetElem == vetTam - 1){
        vetTam *= 2;
        entrVet = alocVetNovo(entrVet, vetTam);
        testVet = alocVetNovo(testVet, vetTam);
    }
    if(sinal < 0) sinal = 1;
    vetElem++;
}

//notifica para as outras que essa thread acabou seu trabalho
void sinalizaTermino(long int id){
    pthread_mutex_lock(&mutex); //inicio da secao critica

    nThreadsOn--; //decremento no numero de threads ativas para a proxima fase
    pthread_cond_broadcast(&cond_prox); //sinaliza a todas as threads

    pthread_mutex_unlock(&mutex); //fim da secao critica
}


//sincronizacao das threads leitoras
void leitSinc(long int id){
    if(leitr == nThreadsOn){
        pthread_cond_broadcast(&cond_prox); //sinaliza a todas as threads
    }
    while(leitr < nThreadsOn){
        pthread_cond_wait(&cond_prox, &mutex);
    }
}

//sincronizacao das threads escritoras
void escrSinc(long int id){
    if(escr == nThreadsOn){
        pthread_cond_broadcast(&cond_prox); //sinaliza a todas as threads
        leitr = 0;
    }
    if(escr < nThreadsOn){
        pthread_cond_wait(&cond_prox, &mutex);
    }

}

void * calcularSomaParcial(void* args){
    long int id = (long int) args;
    int aux;
    for(int i = 1; i < vetTam;i *= 2){
        escr = 0;

        if(id - i < 0) { sinalizaTermino(id); break;}

        pthread_mutex_lock(&mutex);//inicio da secao critica

        aux = *(entrVet + id - i);
        leitr++;
        leitSinc(id);
        *(entrVet+id) += aux;
        escr++;
        escrSinc(id);

        pthread_mutex_unlock(&mutex); //fim da secao critica
    }
    pthread_exit(NULL);
}

//resultado
void printResult(){
    printf("Soma parcial dos elementos do vetor:\n");

    for(int j = 0; j < vetElem; j++){
        printf("vetor[%d] = %d \n", j, *(entrVet + j));
    }

}

//verificacao
void verif(){
    printf("\n\nVerificando:\n");
    for(int j = 0; j < vetElem; j++){
        *(testVet + j) += *(testVet + j - 1);
        if(*(testVet + j) == *(entrVet + j)){
            printf("vetorTeste[%d] = %d compara com %d ✓\n", j, *(testVet + j), *(entrVet + j));
        }else{
            printf("vetor[%d] = %d compara com %d x\n", j, *(testVet + j), *(entrVet + j));

        }
    }

}

//fluxo principal
int main(){
    char entrada;
    pthread_t *tid;

    entrVet = alocVet(vetTam);
    testVet = alocVet(vetTam);

    printf("Insira 2^n numeros separados por um espaço (Ex: 1 4 -1 7):\n");

    while(scanf("%c", &entrada) && (entrada != '\n')){//aceita entradas ate receber enter
        if(entrada == ' ') continue;
        if(entrada == '-') {
            sinal *= -1; continue;
        }
        addEntrVet(entrVet, entrada);
    }

    nThreadsOn = vetElem; //quantidade de threads inicialmente ativas
    pthread_mutex_init(&mutex, NULL); //inicializa a variavel de lock
    pthread_cond_init(&cond_prox, NULL); //inicializa a variavel condicional

    //alocacao de memoria para a criacao das threads
    tid = (pthread_t *) malloc(sizeof(pthread_t)*vetElem);
    if(tid==NULL){printf("Erro malloc\n"); exit(-1);}

    //criacao das threads
    for(int i = 0; i<vetElem; i++){
        if(pthread_create((tid+i), NULL, calcularSomaParcial, (void *) i)){ 
            printf("Erro pthread_create\n");
            exit(-1);}
    }

    //fluxo principal espera todas as threads acabarem antes de fechar
    for(int i = 0; i<vetElem;i++){
        if(pthread_join(*(tid+i), NULL)) exit(-1);
    }

    printResult();
    verif();

    pthread_mutex_destroy(&mutex);
    free(entrVet);
    free(tid);
}
