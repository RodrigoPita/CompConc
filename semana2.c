/* Multiplicacao de matriz-vetor (considerando matrizes quadradas) */

#include <stdio.h>
#include <stdlib.h>

float *mat; //matriz de entrada
float *vet; //vetor de entrada
float *saida; //vetor de saida

int main(int argc, char* argv[]) {
    int dim; //dimensao da matriz de entrada
    //leitura e avaliacao dos parametros de entrada
    if(argc < 2) {
        printf("Digite: %s <dimensao da matriz>\n", argv[0]);
        return 1;
    }
    dim = atoi(argv[1]);
    //alocacao de memoria para as estruturas de dados
    mat = (float *) malloc(sizeof(float) * dim * dim);
    if (mat == NULL) {printf("ERRO--malloc\n"); return 2;}
    vet = (float *) malloc(sizeof(float) * dim);
    if (vet == NULL) {printf("ERRO--malloc\n"); return 2;}
    saida = (float *) malloc(sizeof(float) * dim);
    if (saida == NULL) {printf("ERRO--malloc\n"); return 2;}
    //inicializacao das estruturas de dados
    
    //multiploacap da matriz pelo vetor
    
    //exibicao dos resultados
    
    //liberacao da memoria
    
    return 0;
}
