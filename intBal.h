#include <stdio.h>

#ifndef INT_BAL
#define INT_BAL

typedef struct{
    long inscricao;
    float nota;
    char estado[3];
    char cidade[51];
    char curso[31];
    int fita;
}Registro;

#define f 20
#define f2 (f * 2)

#define QUANT_REGISTROS 100
#define QUANT_TOTAL 471705
#define TAM_AREA 20

#define tamMemoria 20

#endif