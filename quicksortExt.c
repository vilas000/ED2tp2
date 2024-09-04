#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include <string.h>
#include "quicksortExt.h"

void inicializaLista(TipoArea* area){
    area->tam = 0;
    area->reg = (Registro*)malloc(TAM_AREA * sizeof(Registro));
}

void quicksortExterno(FILE **arqLinf, FILE **arqEinf, FILE **arqLEsup, int esq, int dir){
    int i, j;
    TipoArea area;
    if(dir - esq < 1) return;
    inicializaLista(&area);
    particao(arqLinf, arqEinf, arqLEsup, area, esq, dir, &i, &j);
    if(i - esq < dir - j){
        quicksortExterno(arqLinf, arqEinf, arqLEsup, esq, i);
        quicksortExterno(arqLinf, arqEinf, arqLEsup, j, dir);
    } else {
        quicksortExterno(arqLinf, arqEinf, arqLEsup, j, dir);
        quicksortExterno(arqLinf, arqEinf, arqLEsup, esq, i);
    }
    free(area.reg);
}

void leSup(FILE **arqLEsup, Registro *ultimoLido, int *lSup, bool *ondeLer){
    fseek(*arqLEsup, (*lSup - 1) * sizeof(Registro), SEEK_SET);
    fread(ultimoLido, sizeof(Registro), 1, *arqLEsup);
    (*lSup)--; *ondeLer = false;
}

void leInf(FILE **arqLinf, Registro *ultimoLido, int *lInf, bool *ondeLer){
    fread(ultimoLido, sizeof(Registro), 1, *arqLinf);
    (*lInf)++; *ondeLer = true;
}

void inserirArea(TipoArea *area, Registro *ultimoLido, int *numArea){
    insereItem(*ultimoLido, area); 
    *numArea = obterNumCelOcupadas(*area);
}

void escreveMax(FILE **arqLEsup, Registro R, int *eSup){
    fseek(*arqLEsup, (*eSup - 1) * sizeof(Registro), SEEK_SET);
    fwrite(&R, sizeof(Registro), 1, *arqLEsup);
    (*eSup)--;
}

void escreveMin(FILE **arqEInf, Registro R, int *eInf){
    fwrite(&R, sizeof(Registro), 1, *arqEInf);
    (*eInf)++;
}

void retiraUltimo(TipoArea *area, Registro *R) {
    if (area->tam > 0) {
        *R = area->reg[area->tam - 1];  // Get the last item
        area->tam--;  // Decrease the size of the area
    }
}

void retiraPrimeiro(TipoArea *area, Registro *R) {
    if (area->tam > 0) {
        *R = area->reg[0];  // Get the first item
        area->tam--;  // Decrease the size of the area
        for(int i = 0; i < area->tam; i++){
            area->reg[i] = area->reg[i + 1];
        }
    }
}

void insereItem(Registro ultimoLido, TipoArea *area) {
    int i = area->tam - 1;
    // Find the correct position to insert the new item
    while (i >= 0 && area->reg[i].nota > ultimoLido.nota) {
        area->reg[i + 1] = area->reg[i];
        i--;
    }
    // Insert the new item
    area->reg[i + 1] = ultimoLido;
    area->tam++;
}

int obterNumCelOcupadas(TipoArea area){
    return area.tam;
} 

void retiraMax(TipoArea *area, Registro *R, int *numArea){
    retiraUltimo(area, R);
    *numArea = obterNumCelOcupadas(*area);
}

void retiraMin(TipoArea *area, Registro *R, int *numArea){
    retiraPrimeiro(area, R);
    *numArea = obterNumCelOcupadas(*area);
}

void particao(FILE **arqLinf, FILE **arqEinf, FILE **arqLESup,
              TipoArea area, int esq, int dir, int *i, int *j){
    
    int lSup = dir, eSup = dir, lInf = esq, eInf = esq,
        numArea = 0, limInf = INT_MIN, limSup = INT_MAX;
    bool ondeLer = true; 
    Registro ultimoLido, R;
    fseek(*arqLinf, (lInf - 1) * sizeof(Registro), SEEK_SET);
    fseek(*arqEinf, (eInf - 1) * sizeof(Registro), SEEK_SET);
    *i = esq - 1;
    *j = dir + 1;

    while(lSup >= lInf){
        if(numArea < TAM_AREA - 1){
            if(ondeLer)
                leSup(arqLESup, &ultimoLido, &lSup, &ondeLer);
            else leInf(arqLinf, &ultimoLido, &lInf, &ondeLer);

                inserirArea(&area, &ultimoLido, &numArea);
                continue;
        }
        if(lSup == eSup)
            leSup(arqLESup, &ultimoLido, &lSup, &ondeLer);
            else if(lInf == eInf) leInf(arqLinf, &ultimoLido, &lInf, &ondeLer);
                else if (ondeLer) leSup(arqLESup, &ultimoLido, &lSup, &ondeLer);
                    else leInf(arqLinf, &ultimoLido, &lInf, &ondeLer);

        if(ultimoLido.nota > limSup){
            *j = eSup;
            escreveMax(arqLESup, ultimoLido, &eSup);
            // continue;
        } else if(ultimoLido.nota < limInf){
            *i = eInf;
            escreveMin(arqEinf, ultimoLido, &eInf);
            // continue;
        } else {
            inserirArea(&area, &ultimoLido, &numArea);

            if(eInf - esq < dir - eSup){
                retiraMin(&area, &R, &numArea);
                escreveMin(arqEinf, R, &eInf); 
                limInf = R.nota;
            }
            else {
                retiraMax(&area, &R, &numArea);
                escreveMax(arqLESup, R, &eSup);
                limSup = R.nota; 
            }
        }
    }

    while(eInf <= eSup){
        retiraMin(&area, &R, &numArea);
        escreveMin(arqEinf, R, &eInf);
    }
}

int main(int argc, char *argv[]){

    FILE* arqLESup; 
    FILE* arqLInf; 
    FILE* arqEinf; 
    FILE* output; 
    Registro R;


    if((output = fopen("outputTotal.txt", "w")) == NULL)
        exit(1);

    if((arqLInf = fopen("desordenadoTotal.bin", "r+b")) == NULL){
        fclose(output);
        exit(1);
    }
        
    if((arqEinf = fopen("desordenadoTotal.bin", "r+b")) == NULL){
        fclose(arqLInf);
        exit(1);}

    if((arqLESup = fopen("desordenadoTotal.bin", "r+b")) == NULL){
        fclose(arqLInf);
        fclose(arqEinf);
        exit(1);}
    
    quicksortExterno(&arqLInf, &arqEinf, &arqLESup, 1, QUANT_REGISTROS);

    fflush(arqLInf); fclose(arqEinf); fclose(arqLESup);

    fseek(arqLInf, 0, SEEK_SET);

    int i = 1;
    while(fread(&R, sizeof(Registro), 1, arqLInf)){
        // printf("Registro %d: %ld %.2f %s %s %s\n", i, R.inscricao, R.nota, R.estado, R.cidade, R.curso);
        fprintf(output, "%08ld %05.1f %-2s %-60s %-30s\n", 
                R.inscricao,
                R.nota,
                R.estado,
                R.cidade,
                R.curso);
        i++;
    }


    fclose(output);
    fclose(arqLInf);
    return 0;
}
