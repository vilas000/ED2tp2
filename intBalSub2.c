#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "intBalSub.h"

// Funcao de ordenacao Insertion Sort
void insertionSort(RegistroSub arr[], int n)
{
    for (int i = 1; i < n; i++)
    {
        RegistroSub key = arr[i];
        int j = i - 1;
        while (j >= 0 && arr[j].registro.nota > key.registro.nota)
        {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = key;
    }
}

void GeraFitas(FILE **fitas, RegistroSub *interna, Registro pulaBloco)
{

    FILE *arqBin = fopen("desordenadoTotal.bin", "r+b");
    if (arqBin == NULL)
    {
        printf("Erro ao abrir o arquivo desordenadoTotal.bin");
        free(interna);
        exit(1);
    }
    // Inicializa o array de registros
    for (int i = 0; i < tamMemoria; i++)
    {
        interna[i].verificaTrava = false;
    }

    for (int i = 0; i < tamMemoria; i++)
    {
        if (!fread(&interna[i].registro, sizeof(Registro), 1, arqBin))
        {
            printf("Erro ao ler o arquivo");
            fclose(arqBin);
            free(interna);
            for (int j = 1; j <= f2; j++)
            {
                fclose(fitas[j]);
                free(fitas);
                exit(1);
            }
        }
    }
    for (int j = 0; j < tamMemoria; j++)
    {
        printf("Nota: %.2f, trava: %s\n", interna[j].registro.nota, interna[j].verificaTrava ? "true" : "false");
    }
    int i = 1;
    int ultimaSaida = 0;
    float notasaiu = -3;
    bool verifica = true;
    long cont = 0;
    int contafim;
    while (verifica)
    {
        cont++;
        printf("Interacao: %ld \n", cont);

        // Verifica se a nota atual e menor que a ultima nota que saiu para travar o registro
        if (interna[ultimaSaida].registro.nota < notasaiu)
        {
            interna[ultimaSaida].verificaTrava = true;
        }
        printf("\nSituacao: %s\n", interna[ultimaSaida].verificaTrava ? "true" : "false");
        // Ordena internamente
        insertionSort(interna, tamMemoria);

        // Conta quantos registros estao travados
        int contador = 0;
        for (int j = 0; j < tamMemoria; j++)
        {
            if (interna[j].verificaTrava == true)
            {
                contador++;
            }
        }
        printf("Debug: Fitas travadas - %d\nFita atual: %d\n", contador, i);

        // Se todos estiverem travados, reseta as travas e muda para a proxima fita
        if (contador == tamMemoria)
        {
            printf("Debug: inseriu o pulaBloco\n");
            fwrite(&pulaBloco, sizeof(Registro), 1, fitas[i]);
            i++;
            if (i > tamMemoria)
            {
                printf("Debug: resetou as fitas\n");
                i = 1;
            }
            // Reseta as travas
            for (int j = 0; j < tamMemoria; j++)
            {
                interna[j].verificaTrava = false;
            }
        }

        // Grava os registros nao travados na fita atual
        for (int j = 0; j < tamMemoria; j++)
        {
            if (!interna[j].verificaTrava && interna[j].registro.nota >= 0)
            {
                printf("debug: fita: %d Nota que saiu -- %.2f\n", i, interna[j].registro.nota);
                fwrite(&(interna[j].registro), sizeof(Registro), 1, fitas[i]);
                ultimaSaida = j;
                notasaiu = interna[j].registro.nota;
                break;
            }
        }

        if (feof(arqBin))
        {
            contafim = 0;
            interna[ultimaSaida].registro.nota = -1;
            for (int j = 0; j < tamMemoria; j++)
            {
                if (interna[j].registro.nota == -1)
                {
                    contafim++;
                }
            }
            if (contafim == tamMemoria)
            {
                fwrite(&pulaBloco, sizeof(Registro), 1, fitas[i]);
                verifica = false;
                printf("acabou\n");
                continue;
            }
        }
        else
        {
            if (fread(&(interna[ultimaSaida].registro), sizeof(Registro), 1, arqBin) == 1)
                printf("Proxima nota lida: %.2f\n", interna[ultimaSaida].registro.nota);
        }
    }
    fclose(arqBin);
}

void heap(RegistroSub *interna, float *menorNota, int *posMenorNota)
{
    *menorNota = -2;
    *posMenorNota = 0;
    for(int i = 0; i < tamMemoria; i++){
        if(interna[i].registro.nota >=0){
            *menorNota = interna[i].registro.nota;
            *posMenorNota = i;
            break;
        }
    }
    if (*menorNota == -2){
        return;
    }
    for (int i = 0; i < tamMemoria; i++)
    {
        if (interna[i].registro.nota < *menorNota && interna[i].registro.nota >= 0)
        {
            *menorNota = interna[i].registro.nota;
            *posMenorNota = i;
        }
    }
    printf("[funcao Heap] : Menor Nota: %.2f\n", *menorNota);

}

void Intercala(FILE **fitas, RegistroSub *interna, Registro pulaBloco)
{
    bool chave = 0;
    // bool verificafim[41] = {false};
    int contaBlocos[41] = {0};
    int posFitaIntercalada;

    while (1)
    {
        // inicializacao de variaveis para auxiliar no decorrer co codigo
        float menorNota;
        int posMenorNota;
        int contaFimBloco = 0;
        int contaFimFita = 0;
        int cout = 0;

        // a chave indica se a leitura sera feita nas fitas de 1 a 20 ou das fitas 21 a 40
        if (chave == 0)
        {
            // inicializando o indice das fitas de saida
            int saida = 21;
            // lendo o primeiro item de cada uma das fitas e armazenando na memoria principal
            for (int i = 0; i < tamMemoria; i++)
            {
                fread(&(interna[i].registro), sizeof(Registro), 1, fitas[i + 1]);
                printf("Fita: %d\nNota: %.2f\n", i + 1, interna[i].registro.nota);
            }

            // while para realizar a intercalacao
            while (1)
            {
                // chamada da funcao heap para obter a menor nota lida
                heap(interna, &menorNota, &posMenorNota);

                // escrevendo o item obtido pelo heap na fita de saida
                fwrite(&interna[posMenorNota].registro, sizeof(Registro), 1, fitas[saida]);

                // lendo proximo item da fita que saiu
                if (interna[posMenorNota].registro.nota != -1)
                {
                    fread(&(interna[posMenorNota].registro), sizeof(Registro), 1, fitas[posMenorNota + 1]);
                    printf("Fita: %d\nNota: %.2f\n", posMenorNota, interna[posMenorNota].registro.nota);
                }
                contaFimBloco = 0;
                // verificando quantas fitas tiveram o bloco lido por completo
                for (int i = 0; i < f; i++)
                {
                    // verifica nas fitas se existe algum registro -1, caso sim, incrementa o contador
                    if (interna[i].registro.nota == -1)
                    {
                        // incrementa a quantidade de blocos que chegaram ao fim
                        contaFimBloco++;
                        //printf("contaFimBloco: %d\n", contaFimBloco);
                    }
                    else
                        printf("fita que nao esta com -1: [%d]\n", i + 1);
                }

                // se toda memoria interna estiver com -1 entao 1 bloco de cada fita ja foi lido
                if (contaFimBloco == tamMemoria)
                {
                    cout++;
                    printf("contaFimBloco: %d\n", cout);
                    // atualizando a quantidade de blocos lidos em cada fita de saida
                    contaBlocos[saida]++;

                    // Escrevemos o valor da variavel pulaBloco que possui o valor -1 na nota, para sinalizar o final do bloco
                    fwrite(&pulaBloco, sizeof(Registro), 1, fitas[saida]);

                    // A variavel e saida e usada para mudar a fita de saida
                    saida++;
                    // leitura dos proximos itens e amazenando na memoria
                    for (int i = 0; i < tamMemoria; i++)
                    {
                        // Usado para ler a proxima fita
                        fread(&(interna[i].registro), sizeof(Registro), 1, fitas[i + 1]);
                        printf("Fita: %d\nNota: %.2f\n", i + 1, interna[i].registro.nota);
                    }
                }
                // setando a variavel que ira verificar quantas fitas ja foram lidas por completo
                contaFimFita = 0;

                //Registro aux;

                // contando quantas fitas chegaram no fim
                for (int i = 1; i <= f; i++)
                {
                    if (feof(fitas[i]))
                    {
                        contaFimFita++;
                        //printf("[%d] -> %d\n", i, contaFimFita);
                    }
                    else
                    {
                        // printf("fita pos[%d] nao concluida\n", i);
                    }
                }
                // printf("debug: while 1 de dentro\n");
                // se a quantidade de fitar que chegou no fim forem iguais a quantidade de fitas de entrada finalizamos o while
                if (contaFimFita == tamMemoria)
                {
                    printf("debug: saiu do while 1 de dentro\n");
                    break;
                }
            }

            contaFimFita = 0;
            // verificando se temos somente uma fita de saida com apenas 1 bloco
            for (int i = 1; i <= f; i++)
            {
                // verifica se chegamos no final da fita
                if (feof(fitas[i]))
                {
                    // Incrementa quando chegamos no final da fita
                    contaFimFita++;
                }
                else
                    posFitaIntercalada = i;
            }
            printf("debug: while de fora\n Contafitas: %d\n", contaFimFita);
            if (contaFimFita == f - 1 && contaBlocos[posFitaIntercalada] == 1)
            {
                printf("debug: saiu do while de fora\n");
                break;
            }
            else if(contaFimFita == f)
                break;
        }
        else
        {
            // inicializando o indice das fitas de saida
            int saida = 1;

            // lendo o primeiro item de cada uma das fitas e armazenando na memoria principal
            for (int i = 0; i < tamMemoria; i++)
            {
                fread(&(interna[i].registro), sizeof(Registro), 1, fitas[i + 21]);
            }

            // while para realizar a intercalacao
            while (1)
            {
                // chamada da funcao heap para obter a menor nota lida
                heap(interna, &menorNota, &posMenorNota);

                // escrevendo a menor nota na fita de saida
                fwrite(&interna[posMenorNota].registro, sizeof(Registro), 1, fitas[saida]);

                // lendo proximo item da fita que saiu
                fread(&(interna[posMenorNota].registro), sizeof(Registro), 1, fitas[posMenorNota + 21]);

                // verificando quantas fitas tiveram o bloco lido por completo
                for (int i = 0; i < tamMemoria; i++)
                {
                    if (interna[i].registro.nota == -1)
                    {
                        contaFimBloco++;
                    }
                }
                // se toda memoria interna estiver com -1 entao 1 bloco de cada fita ja foi lido
                if (contaFimBloco == tamMemoria)
                {
                    // atualizando a quantidade de blocos lidos em cada fita de saida
                    contaBlocos[saida]++;

                    // Escrevemos o valor da variavel pulaBloco que possui o valor -1 na nota, para sinalizar o final do bloco
                    fwrite(&pulaBloco, sizeof(Registro), 1, fitas[saida]);

                    // A variavel e saida e usada para mudar a fita de saida
                    saida++;

                    for (int i = 0; i < tamMemoria; i++)
                    {
                        if (fread(&(interna[i].registro), sizeof(Registro), 1, fitas[i + 21]) != 1)
                        {
                            continue;
                        }
                    }
                }
                for (int i = 21; i <= f2; i++)
                {
                    if (feof(fitas[i]))
                    {
                        contaFimFita++;
                    }
                }
                printf("debug: while 2 de dentro\n");
                if (contaFimFita == tamMemoria)
                {
                    printf("debug: saiu do while 2 de dentro\n");
                    break;
                }

                contaFimFita = 0;
                for (int i = 1; i <= f; i++)
                {
                    if (feof(fitas[i]))
                    {
                        contaFimFita++;
                    }
                    else
                        posFitaIntercalada = i;
                }
                printf("debug: while de fora\n Contafitas: %d\n", contaFimFita);
                if (contaFimFita == f - 1 && contaBlocos[posFitaIntercalada] == 1)
                {
                    printf("debug: saiu do while de fora\n");
                    break;
                }
                else if(contaFimFita == f)
                    break;
            }
        }

        /*if(contaFimFita == 19 && contaFimBloco == 1){
            break;
        }*/
    }
}

int main(int argc, char *argv[])
{
    char nome[11] = "fita";
    char final[5] = ".bin";
    char index[3];
    char temp[11];
    Registro pulaBloco = {0, -1, "", "", ""}; // Inicializacao do bloco de pulo

    RegistroSub *interna = (RegistroSub *)malloc(tamMemoria * sizeof(RegistroSub));

    if (interna == NULL)
    {
        printf("Erro ao alocar memoria para interna");
        exit(1);
    }

    FILE **fitas = (FILE **)malloc((f2 + 1) * sizeof(FILE *));
    if (fitas == NULL)
    {
        printf("Erro ao alocar memoria para fitas");
        free(interna);
        exit(1);
    }

    // Abre as fitas
    for (int i = 1; i <= f2; i++)
    { // Comecando de 1
        strcpy(temp, nome);
        sprintf(index, "%d", i);
        strcat(temp, index);
        strcat(temp, final);
        fitas[i] = fopen(temp, "w+b");
        if (fitas[i] == NULL)
        {
            printf("Erro ao abrir a fita %d", i);
            free(interna);
            for (int j = 1; j < i; j++)
            {
                fclose(fitas[j]);
            }
            free(fitas);
            exit(1);
        }
    }

    // Gera as fitas (funcao principal)
    GeraFitas(fitas, interna, pulaBloco);

    for (int j = 1; j <= f2; j++)
    {
        rewind(fitas[j]);
    }

    Intercala(fitas, interna, pulaBloco);

     for (int j = 1; j <= f2; j++){
        rewind(fitas[j]);
    }
    Registro aux;
    printf("Debug: printando a fita 1\n");
    while (fread(&aux, sizeof(Registro), 1, fitas[21]) == 1)
    {
        if(aux.nota >=0){
            printf("Nota: %.2f \n", aux.nota);
        }
        
    }


    // Fecha todas as fitas e libera memoria
    for (int j = 1; j <= f2; j++)
    {
        fclose(fitas[j]);
    }

    free(fitas);
    free(interna);

    return 0;
}
