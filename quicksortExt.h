#include <stdio.h>

#ifndef QUICKSORT_H
#define QUICKSORT_H

typedef struct{
    long inscricao;
    float nota;
    char estado[3];
    char cidade[51];
    char curso[31];
}Registro;

#define QUANT_REGISTROS 100
#define TAM_AREA 20

typedef struct{
    Registro* reg;
    int tam;
}TipoArea;

typedef Registro* tipoApontador;

void insereItem(Registro ultimoLido, TipoArea *area);
void retiraUltimo(TipoArea *area, Registro *R);
void inicializaLista(TipoArea* area);
void quicksortExterno(FILE **arqLinf, FILE **arqEinf, FILE **arqLEsup, int esq, int dir);
void leSup(FILE **arqLEsup, Registro *ultimoLido, int *lSup, bool *ondeLer);
void leInf(FILE **arqLinf, Registro *ultimoLido, int *lInf, bool *ondeLer);
void inserirArea(TipoArea *area, Registro *ultimoLido, int *numArea);
void escreveMax(FILE **arqLEsup, Registro R, int *eSup);
void escreveMin(FILE **arqEInf, Registro R, int *eInf);
void retiraMax(TipoArea *area, Registro *R, int *numArea);
void retiraMin(TipoArea *area, Registro *R, int *numArea);
void particao(FILE **arqLinf, FILE **arqEinf, FILE **arqLESup,
              TipoArea area, int esq, int dir, int *i, int *j);
int obterNumCelOcupadas(TipoArea area);

#endif 