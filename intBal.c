#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>
#include <math.h>
#include <float.h>
#include "intBal.h"

//nome fitas: fita1 fita2 ...

int somaAtivos(int* ativos){
    int total = 0;
    for(int i = 0; i < f; i++)
        total += ativos[i];
    return total;
}

void troca(Registro* a, Registro* b) {
    Registro temp = *a;
    *a = *b;
    *b = temp;
}

void constroiHeap(Registro* vetor, int n) {
    int idxInicial = (n / 2) - 1;
    for (int i = idxInicial; i >= 0; i--) {
        heapify(vetor, n, i);
    }
}

void heapify(Registro* vetor, int n, int i) {
    int menor = i;        // Initialize menor as root
    int esq = 2 * i + 1;    // esq child index
    int dir = 2 * i + 2;   // dir child index

    // If esq child is smaller than root
    if (esq < n && vetor[esq].nota < vetor[menor].nota)
        menor = esq;

    // If dir child is smaller than menor so far
    if (dir < n && vetor[dir].nota < vetor[menor].nota)
        menor = dir;

    // If menor is not root
    if (menor != i) {
        troca(&vetor[i], &vetor[menor]);  // Swap root and menor

        // Recursively heapify the affected subtree
        heapify(vetor, n, menor);
    }
}

void trocaFitas(bool entrada, FILE** fitas){
    
    char nome[11] = "fita"; 
    char final[5] = ".bin";
    char index[3];
    char temp[11];

    for(int i = 0; i < f2; i++){

        fclose(fitas[i]);
        strcpy(temp, nome); // temp == "nome"
        sprintf(index, "%d", i);
        strcat(temp, index); strcat(temp, final); 
            if(entrada){ 
                fitas[i] = fopen(temp, "wb");
            } else {
                fitas[i] = fopen(temp, "rb");  // Reopen in read mode before reading
            }
            if(i == f) entrada = !entrada;  // Reopen in read mode before reading
    }
}


//insertion para ordenar blocos
void insertionSort(Registro* vetor, int n) {
    for (int i = 1; i < n; i++) {
        Registro key = vetor[i];
        int j = i - 1;
        while (j >= 0 && vetor[j].nota > key.nota) {
            vetor[j + 1] = vetor[j];
            j = j - 1;
        }
        vetor[j + 1] = key;
    }
}






























void criaRuns(FILE* arqBin, int *numBlocos){

    Registro interna[tamMemoria];
    int i;
    int contFita = 0;
    int contRegistros = 0;
    char nomeFita[20];
    Registro R;

    FILE* fitas[f];

    for(i = 0; i < f; i++){
        sprintf(nomeFita, "fita_in%d.bin", i);
        fitas[i] = fopen(nomeFita, "wb");
        if (fitas[i] == NULL) {
            printf("Erro ao criar a fita de saída %d.\n", i);
            return;
        }
    }

    printf("Geracao de fitas feita\n");

    //FITAS DE ENTRADA
    //gerando os blocos ordenados dentro das fitas de entrada
    for(int j = 0; j < 400; j++){

        fread(&R, sizeof(Registro), 1, arqBin);

        interna[contRegistros++] = R;

        if(contRegistros == tamMemoria){
            //metodo de ordenacao pra ordenar o que ta em variavel "interna" e armazenar em fita[i]
            insertionSort(interna, tamMemoria);
            
            //escrevendo bloco ordenado em fita[i]
            for(i = 0; i < tamMemoria; i++)
                fwrite(&interna[i], sizeof(Registro), 1, fitas[contFita % f]);

            contRegistros = 0;
            contFita++;
        }
    }
    //fechar e abrir fitas toda hora pra serem de escrita ou de leitura

    // Grava os números restantes se houver menos que o limite
    if (contRegistros > 0) {
        insertionSort(interna, contRegistros);
        for (i = 0; i < contRegistros; i++) {
            fwrite(&interna[i], sizeof(Registro), 1, fitas[contFita % f]);
        }
        contFita++;
    }

    *numBlocos = contFita;

    // os dados todos ja foram pegos
    // Fecha as fitas
    for (i = 0; i < f; i++) {
        fclose(fitas[i]);
    }
    fclose(arqBin);
}

void intercalacaoBalanceada(int numBlocos){

    printf("entra em int bal\n");
    FILE *fitasIn[f], *fitasOut[f], *fitaFinal;
    char nomeFita[20];
    int i;
    Registro R;

    // Abre fitas de entrada para leitura em binário
    for (i = 0; i < f; i++) {
        sprintf(nomeFita, "fita_in%d.bin", i);
        if((fitasIn[i] = fopen(nomeFita, "rb")) == NULL){
            printf("Erro ao abrir a fita de entrada %d.\n", i);
            exit(1);
        }
    }

    // Abre fitas de saida para escrita em binário
    for (i = 0; i < f; i++) {
        sprintf(nomeFita, "fita_out%d.bin", i);
        if((fitasOut[i] = fopen(nomeFita, "wb")) == NULL){
            printf("Erro ao abrir a fita de entrada %d.\n", i);
            exit(1);
        }
    }

    //Abre o arquivo binário final para gravação
    fitaFinal = fopen("arquivoBinarioFinal.bin", "wb");
    if (fitaFinal == NULL) {
        printf("Erro ao abrir o arquivo binario final.\n");
        for (i = 0; i < f; i++) {
            fclose(fitasIn[i]);
        }
        return;
    }

    //NESSE PONTO O VETOR INTERNO ESTA COM O MENOR ELEMENTO DE CADA RUN DOS PRIMEIROS BLOCOS HAHAHAHAHAHHAHAHAHAHAHAHA

    int colunasTotais = ceil((float)numBlocos / f);
    int limiteBloco = f;
    int j;
    bool trocaFita = true;

    printf("chega ate a funcao que eu quero");

    while(numBlocos > 1){

        printf("\nNum blocos: %d\n", numBlocos);

        // Intercala runs das fitas de entrada nas fitas de saída
        for(i = 1; i <= colunasTotais; i++){

            // Vetor para armazenar o próximo elemento de cada fita
            Registro* menores = (Registro*)calloc(f, sizeof(Registro));
            int* ativos = (int*)malloc(f * sizeof(int));          // Indica se ainda há registros para ler de cada fita
            int* contadores = (int*)malloc(f * sizeof(int));      //Conta quantos elementos de um bloco ja foram lidos

            for (int d = 0; d < f; d++){
                // menores[d].nota = 0;
                ativos[d] = 1;
                contadores[d] = 0;
            }
            
            Registro menor; // = { 0 };
            menor.nota = FLT_MAX;
            int fitaMenor = -1;
            // fim = 1;

            // Lê o primeiro registro de cada fita
            for (j = 0; j < f; j++) {
                if (fread(&R, sizeof(Registro), 1, fitasIn[j]) == 1) {
                    menores[j] = R;
                    (contadores[j])++;
                } else {
                    ativos[j] = 0;  // Marca que não há mais registros nesta fita
                }
            }

            while(somaAtivos(ativos) > 0){
                // printf("%d \n", somaAtivos(ativos));

                for(int banana = 0; banana < f; banana++)
                    printf("%d ", contadores[banana]);
                printf("\n");

                // Determina o menor registro entre as fitas ativas
                for (j = 0; j < f; j++) {
                    printf("menores vetor %f - menor atual %f\n", menores[j].nota, menor.nota);
                    if ((ativos[j] == 1) && (fitaMenor == -1 || menores[j].nota < menor.nota)) {
                        menor = menores[j];
                        fitaMenor = j;
                    }
                }
                printf("fita menor %d\n", fitaMenor);

                //if (fim) break;  // Sai se não houver mais números a serem processados
            
                // Grava o menor número na fita de saída
                // printf("Valor de i - 1: %d\n", i - 1);
                if((fwrite(&menor, sizeof(Registro), 1, fitasOut[i - 1])) != 1)
                    printf("erro em inserir em OUT!!!\n");
  

                // Lê o próximo número da fita de onde foi extraído o menor
                if (ativos[fitaMenor] == 1) {

                    if(fread(&R, sizeof(Registro), 1, fitasIn[fitaMenor]) < 1){
                        ativos[fitaMenor] = 0;
                        menores[fitaMenor].nota = FLT_MAX;
                        break;
                    }

                    (contadores[fitaMenor])++;

                    //garante que nao sera lido depois de chegar no limite do bloco
                    if(contadores[fitaMenor] >= limiteBloco) {
                        ativos[fitaMenor] = 0;
                        menores[fitaMenor].nota = FLT_MAX;  // Marca como fim daquela fita
                    } 
                    menores[fitaMenor] = R;
                } else {
                    //ativos[fitaMenor] = 0;
                    menores[fitaMenor].nota = FLT_MAX;  // Marca como fim daquela fita
                }
            }

            printf("teste importante");

        }

        limiteBloco *= f;
        numBlocos = ceil(numBlocos / (float)f);
        for (int j = 0; j < f; j++) {
            fclose(fitasIn[j]);
            fclose(fitasOut[j]);
        }

        if(trocaFita){

            // Abre fitas de entrada para leitura em binário
            for (j = 0; j < f; j++) {
                sprintf(nomeFita, "fita_out%d.bin", j);
                if((fitasIn[j] = fopen(nomeFita, "rb")) == NULL){
                    printf("Erro ao abrir a fita de entrada %d.\n", j);
                    exit(1);
                }
            }

            // Abre fitas de saida para escrita em binário
            for (j = 0; j < f; j++) {
                sprintf(nomeFita, "fita_in%d.bin", j);
                if((fitasOut[j] = fopen(nomeFita, "wb")) == NULL){
                    printf("Erro ao abrir a fita de entrada %d.\n", j);
                    exit(1);
                }
            }

        } else {

            // Abre fitas de entrada para leitura em binário
            for (j = 0; j < f; j++) {
                sprintf(nomeFita, "fita_in%d.bin", j);
                if((fitasIn[j] = fopen(nomeFita, "rb")) == NULL){
                    printf("Erro ao abrir a fita de entrada %d.\n", j);
                    exit(1);
                }
            }

            // Abre fitas de saida para escrita em binário
            for (j = 0; j < f; j++) {
                sprintf(nomeFita, "fita_out%d.bin", j);
                if((fitasOut[j] = fopen(nomeFita, "wb")) == NULL){
                    printf("Erro ao abrir a fita de entrada %d.\n", j);
                    exit(1);
                }
            }
        } 
        trocaFita = !trocaFita;
    } 

    fclose(fitaFinal);
}


int main(int argc, char *argv[]){

    FILE* arqBin;
    FILE* output; 
    int i, numBlocos;

    if((output = fopen("outputIntBal.txt", "w")) == NULL)
        exit(1);

    if((arqBin = fopen("desordenadoTotal.bin", "rb")) == NULL)
        exit(1);

    criaRuns(arqBin, &numBlocos);

    //ATE AQUI TA TUDO CERTO

    intercalacaoBalanceada(numBlocos);

    FILE* temp;
    Registro R;

    if((temp = fopen("arquivoBinarioFinal.bin", "rb")) == NULL)
        exit(1);

    i = 1;
    while(fread(&R, sizeof(Registro), 1, temp)){
        // printf("Registro %d: %ld %.2f %s %s %s\n", i, R.inscricao, R.nota, R.estado, R.cidade, R.curso);
        fprintf(output, "%08ld %05.1f %-2s %-60s %-30s\n", 
                R.inscricao,
                R.nota,
                R.estado,
                R.cidade,
                R.curso);
        i++;
    }  

    fclose(temp);  
    fclose(output);  

    return 0;
}


