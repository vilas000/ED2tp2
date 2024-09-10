#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>
#include <math.h>
#include "intBal.h"

//nome fitas: fita1 fita2 ...

//insertion para ordenar blocos
void insertionSort(Registro arr[], int n) {
    for (int i = 1; i < n; i++) {
        Registro key = arr[i];
        int j = i - 1;
        while (j >= 0 && arr[j].nota > key.nota) {
            arr[j + 1] = arr[j];
            j = j - 1;
        }
        arr[j + 1] = key;
    }
}


int main(int argc, char *argv[]){

    // criando todos os arquivos binarios que irao representar as fitas
    char nome[11] = "fita"; 
    char final[5] = ".bin";
    char index[3];
    char temp[11];

    // long quantConsulta = QUANT_REGISTROS / 5;
    Registro* interna = (Registro*)malloc(tamMemoria * sizeof(Registro));

    int i, contaBlocosUtilizados = 0;
    int quantBlocosSuc;

    FILE* arqBin;

    if((arqBin = fopen("desordenadoCem.bin", "r+b")) == NULL)
        exit(1);

    FILE** fitas = (FILE**)malloc(f2 * sizeof(FILE*));
    for(i = 1; i <= f2; i++){
        strcpy(temp, nome); // temp == "nome"
        sprintf(index, "%d", i);
        strcat(temp, index); strcat(temp, final); 
        fitas[i] = fopen(temp, "wb");
    }

    //FITAS DE ENTRADA
    //gerando os blocos ordenados dentro das fitas de entrada
    i = 1;
    while(fread(interna, sizeof(Registro), tamMemoria, arqBin)){
        //lalalaaa metodo de ordenacao pra ordenar o que ta em variavel "interna" e armazenar em fita[i]
        insertionSort(interna, tamMemoria);
        
        //escrevendo bloco ordenado em fita[i]
        fwrite(interna, sizeof(Registro), tamMemoria, fitas[i]);

        i++;
        contaBlocosUtilizados++;
        if(i > f) i = 1;
    }

    //for com rewind das fitas
    for(int i = 0; i < f; i++){
        rewind(fitas[i]);
    }

    //FITAS DE SAIDA
    bool entrada = true;

    // ddasds dsadsadsa dsadsa rerwwerrew
    // dsdas dsadsadas dsadsa  dsffdsfds
    // dsadas dsadasdsa sdadsa

    // ddasdsdsdasdsadas rerwwerrewdsffdsfds
    // dsadsadsadsadsadasdsadasdsa 
    // dsadsadsadsasdadsa
    // 11 / 3 = 4

    // i = f + 1;
    int inicio, fim, blocosUltColuna;
    while (contaBlocosUtilizados != 1){

        quantBlocosSuc = ceil((float)contaBlocosUtilizados / f); 
        blocosUltColuna = contaBlocosUtilizados % f;

        contaBlocosUtilizados = 0;
        if(entrada){
            inicio = 1; fim = f;} 
                else{
            inicio = f + 1; fim = f2;}

            bool heapFeito = false;
            int* quantLidos = (int*)calloc(f, sizeof(int));
            int contUtilizadas = 0;

        //cada coluna que forma um novo bloco
        for(int j = 0; j < quantBlocosSuc; j++){

            if(j = quantBlocosSuc - 1 && blocosUltColuna != 0) 
                if(entrada) fim = blocosUltColuna;
                else fim = blocosUltColuna + f;
            
                //passar todos os primeiros elementos de cada bloco para "interna"
            for(int k = inicio; k <= fim; k++){
                if(fread(interna, sizeof(Registro), 1, fitas[k]))
                    interna[k].fita = k;
            }

            while (contUtilizadas <= f){
                //fazer o heap
                if(!heapFeito){
                    makeHeap(interna);
                    heapFeito = true;
                }
                
                Registro menor = interna[0];
                
                //mandar o menor elemento pra fita de saida
                fwrite(&menor, sizeof(Registro), 1, fitas[i]);

                //insere mais um no heap, da fita do qual o que saiu era
                if(quantLidos[menor.fita] > f)
                    fread(&interna[0], sizeof(Registro), 1, fitas[menor.fita]);
                else {
                    interna[0].nota = INT_MAX;
                    contUtilizadas++;
                }

                //refaz o heap
                heapify(interna);
            }
            contaBlocosUtilizados++; entrada = !entrada;
        }
    }

    if(entrada) {} //arquivo esta ordenado na fita[1]
    else{} //else arquivo esta ordenado na fita[f + 1]

}


