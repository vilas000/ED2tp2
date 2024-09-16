#include <stdio.h>

#ifndef INT_BAL_SUB
#define NT_BAL_SUB

#define f 20
#define f2 (f * 2)

#define QUANT_REGISTROS 100
#define QUANT_TOTAL 471705
#define TAM_AREA 20

#define tamMemoria 20

typedef struct
{
    long inscricao;
    float nota;
    char estado[3];
    char cidade[51];
    char curso[31];
} Registro;

typedef struct
{
    Registro registro;
    bool verificaTrava;
} RegistroSub;

#endif