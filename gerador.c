#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct{
    long inscricao;
    float nota;
    char estado[3];
    char cidade[51];
    char curso[31];
}Registro;

typedef Registro* tipoApontador;

//Quantidade de registros desejada
#define QUANT_REGISTROS 100
#define QUANT_TOTAL 471705

int main (int argc, char *argv[]){

    FILE* arqTxt;
    FILE* arqBin; 
    int i = 0;
    Registro* reg =(Registro*)malloc(QUANT_REGISTROS * sizeof(Registro));

    if((arqTxt = fopen("PROVAO.txt", "r")) == NULL)
        exit(1);

    if((arqBin = fopen("desordenadoCem.bin", "wb")) == NULL){
        fclose(arqTxt);
        exit(1);
    }

    while(i < QUANT_REGISTROS && fscanf(arqTxt, "%ld %f %2s", &reg[i].inscricao, &reg[i].nota, reg[i].estado) == 3) {
        //Usar fgets para strings que possam conter espacos
        fgetc(arqTxt); //Consumir o espaco depois de "estado"

        fgets(reg[i].cidade, 51, arqTxt);
        reg[i].cidade[strcspn(reg[i].cidade, "\n")] = '\0';  

        fgets(reg[i].curso, 31, arqTxt);
        reg[i].curso[strcspn(reg[i].curso, "\n")] = '\0';  

        i++;
    }

    char op;
    i = 1;
    printf("1 - Crescente\n2 - Descrescente\n3 - Aleatorio");
    scanf("%c", &op);

    switch(op){
    case '1': //Crescente
        while ((fwrite(reg, sizeof(Registro), 1, arqBin) != 0) && (i <= QUANT_REGISTROS)){
            // printf("%d\n", i); 
            i++;
        }
        break;
    case '2': //Decrescente
        
        break;
    case '3': //Aleatorio
        while ((fwrite(reg, sizeof(Registro), 1, arqBin) != 0) && (i <= QUANT_REGISTROS)){
            printf("%d\n", i); 
            i++;
        }
        break;
    default:

        break;
    }


    fclose(arqTxt);
    fclose(arqBin);
    free(reg);

    

    return 0;
}

