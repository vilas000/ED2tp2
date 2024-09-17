#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct{
    long inscricao;
    float nota;
    char estado[3];
    char cidade[51];
    char curso[31];
    int fita;
}Registro;

typedef Registro* tipoApontador;

//Quantidade de registros desejada
#define QUANT_REGISTROS QUANT_TOTAL
#define QUANT_TOTAL 471705

int main (int argc, char *argv[]){

    FILE* arqTxt;
    FILE* arqBin; 
    int i = 0;
    Registro* reg = (Registro*)malloc(QUANT_REGISTROS * sizeof(Registro));

    if((arqTxt = fopen("PROVAO.txt", "r")) == NULL)
        exit(1);

    if((arqBin = fopen("desordenadoTotal.bin", "wb")) == NULL){
        fclose(arqTxt);
        exit(1);
    }

    while(i < QUANT_REGISTROS && fscanf(arqTxt, "%ld %f %2s", &reg[i].inscricao, &reg[i].nota, reg[i].estado) == 3) {
        fgetc(arqTxt); // Consume space after "estado"

        fgets(reg[i].cidade, 51, arqTxt);
        reg[i].cidade[strcspn(reg[i].cidade, "\n")] = '\0';  

        fgets(reg[i].curso, 31, arqTxt);
        reg[i].curso[strcspn(reg[i].curso, "\n")] = '\0';  

        i++;
    }

    // printf("Size of Registro: %u bytes\n", sizeof(Registro));

    fwrite(reg, sizeof(Registro), QUANT_REGISTROS, arqBin);

    fclose(arqTxt);
    fclose(arqBin);
    free(reg);

    return 0;
}
