#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

    // ddasds dsadsadsa dsadsa rerwwerrew
    // dsdas dsadsadas dsadsa  dsffdsfds
    // dsadas dsadasdsa sdadsa
    // 11 / 3 = 4
    i = f + 1;
    quantBlocosSuc = ceil((float)contaBlocosUtilizados / f); 
    for(int j = 0; j < quantBlocosSuc; j++){
        //passar todos os primeiros elementos de cada bloco para "interna"
        for(int k = 1; k <= f; k++){
            if(fread(interna, sizeof(Registro), 1, fitas[k]))
                interna[k].fita = k;
        }
        
        //fazer o heap
        //mandar o menor elemento pra fita de saida
        //insere mais um no heap, da fita do qual o que saiu era
        //refaz o heap

    }




}


