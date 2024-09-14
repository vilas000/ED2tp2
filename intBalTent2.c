#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <math.h>

#define f 20           // Número de fitas de entrada e saída
#define tamMemoria 20  // Quantidade máxima de registros em memória

typedef struct {
    long inscricao;
    float nota;
    char estado[3];
    char cidade[51];
    char curso[31];
} Registro;

// Função para ordenar um bloco de registros usando Insertion Sort
void insertionSort(Registro arr[], int n) {
    int i, j;
    Registro key;
    for (i = 1; i < n; i++) {
        key = arr[i];
        j = i - 1;
        while (j >= 0 && arr[j].nota > key.nota) {
            arr[j + 1] = arr[j];
            j = j - 1;
        }
        arr[j + 1] = key;
    }
}

int somaAtivos(int* ativos) {
    int total = 0;
    for (int i = 0; i < f; i++)
        total += ativos[i];
    return total;
}

// Função para gerar os blocos iniciais (runs)
void criaRuns(FILE* arqBin, int *numBlocos) {
    Registro interna[tamMemoria];
    int i;
    int contFita = 0;
    int contRegistros = 0;
    char nomeFita[20];
    Registro R;
    FILE* fitas[f];

    for (i = 0; i < f; i++) {
        sprintf(nomeFita, "fita_in%d.bin", i);  // Agora todas as fitas seguem o padrão fita_in%d_0.bin
        fitas[i] = fopen(nomeFita, "wb");
        if (fitas[i] == NULL) {
            printf("Erro ao criar a fita de saída %d.\n", i);
            return;
        }
    }

    while (fread(&R, sizeof(Registro), 1, arqBin) == 1) {
        interna[contRegistros++] = R;
        if (contRegistros == tamMemoria) {
            // Ordena os registros em memória
            insertionSort(interna, tamMemoria);
            for (i = 0; i < tamMemoria; i++)
                fwrite(&interna[i], sizeof(Registro), 1, fitas[contFita % f]);

            contRegistros = 0;
            contFita++;
        }
    }

    // Grava os números restantes se houver menos que o limite
    if (contRegistros > 0) {
        insertionSort(interna, contRegistros);
        for (i = 0; i < contRegistros; i++) {
            fwrite(&interna[i], sizeof(Registro), 1, fitas[contFita % f]);
        }
        contFita++;
    }

    *numBlocos = contFita;

    for (i = 0; i < f; i++) {
        fclose(fitas[i]);
    }
    fclose(arqBin);
}

// Função para realizar a intercalação balanceada
void intercalacaoBalanceada(int numBlocos) {
    FILE *fitasIn[f], *fitasOut[f], *arquivoFinal;
    char nomeFita[20];
    int i, j;
    Registro R;

    // int passo = 0;  // Variável para alternar entre fitas de entrada e saída

    // Abre o arquivo final
    // fitaFinal = fopen("arquivoBinarioFinal.bin", "wb");
    // if (fitaFinal == NULL) {
    //     printf("Erro ao abrir o arquivo binário final.\n");
    //     return;
    // }

    while (numBlocos > 1) {
        int numNovosBlocos = 0;

        // Abre fitas de entrada para leitura
        for (i = 0; i < f; i++) {
            sprintf(nomeFita, "fita_in%d.bin", i);  // Abrindo as fitas com o padrão adequado
            if ((fitasIn[i] = fopen(nomeFita, "rb")) == NULL) {
                printf("Erro ao abrir a fita de entrada %d.\n", i);
                exit(1);
            }
        }

        // Abre fitas de saída para escrita
        for (i = 0; i < f; i++) {
            sprintf(nomeFita, "fita_out%d.bin", i);  // Saída também segue o mesmo padrão
            if ((fitasOut[i] = fopen(nomeFita, "wb")) == NULL) {
                printf("Erro ao abrir a fita de saída %d.\n", i);
                exit(1);
            }
        }

        // Processa os blocos de entrada e grava nas fitas de saída
        for (int bloco = 0; bloco < numBlocos; bloco += f) {
            Registro menores[f];
            int ativos[f];
            int contadores[f];

            // Inicializa as estruturas
            for (j = 0; j < f; j++) {
                menores[j].nota = INT_MAX;
                ativos[j] = 0;
                contadores[j] = 0;

                // Lê o primeiro registro de cada fita
                if (fread(&R, sizeof(Registro), 1, fitasIn[j]) == 1) {
                    menores[j] = R;
                    ativos[j] = 1;
                    contadores[j]++;
                }
            }

            // Intercala os registros até que todos os blocos tenham sido processados
            while (somaAtivos(ativos) > 0) {
                // Encontra o menor registro entre os blocos ativos
                int menorIndex = -1;
                for (j = 0; j < f; j++) {
                    if (ativos[j] && (menorIndex == -1 || menores[j].nota < menores[menorIndex].nota)) {
                        menorIndex = j;
                    }
                }

                // Escreve o menor registro na fita de saída
                fwrite(&menores[menorIndex], sizeof(Registro), 1, fitasOut[numNovosBlocos % f]);

                // Atualiza o bloco ativo
                if (fread(&R, sizeof(Registro), 1, fitasIn[menorIndex]) == 1) {
                    menores[menorIndex] = R;
                    contadores[menorIndex]++;
                } else {
                    ativos[menorIndex] = 0;  // Marca o bloco como inativo
                }
            }

            numNovosBlocos++;
        }

        // Fecha as fitas
        for (i = 0; i < f; i++) {
            fclose(fitasIn[i]);
            fclose(fitasOut[i]);
        }

        numBlocos = numNovosBlocos;
        // passo++;
    }

    // Copia o último bloco ordenado para o arquivo final
    sprintf(nomeFita, "fita_out0.bin");  // Considera a primeira fita de saída como final
    arquivoFinal = fopen("arquivoBinarioFinal.bin", "wb");
    FILE *fitaFinal = fopen(nomeFita, "rb");
    if (fitaFinal == NULL || arquivoFinal == NULL) {
        printf("Erro ao abrir arquivos para o passo final.\n");
        return;
    }

    while ((fread(&R, sizeof(Registro), 1, fitaFinal)) == 1){
        fwrite(&R, sizeof(Registro), 1, arquivoFinal);
    }
    
    fclose(fitaFinal);
    fclose(arquivoFinal);
}

// Função para imprimir os dados finais na tela
void imprimeFitaFinal() {
    FILE *fitaFinal = fopen("arquivoBinarioFinal.bin", "rb");
    FILE *output = fopen("lista.txt", "w");

    if (fitaFinal == NULL || output == NULL) {
        printf("Erro ao abrir a fita final para leitura.\n");
        return;
    }

    Registro R;
    printf("Dados ordenados:\n");
    while (fread(&R, sizeof(Registro), 1, fitaFinal)) {
        fprintf(output ,"Inscricao: %ld | Nota: %.2f | Estado: %s | Cidade: %s | Curso: %s\n",
            R.inscricao, R.nota, R.estado, R.cidade, R.curso);
    }

    fclose(fitaFinal);
    fclose(output);
}

int main(int argc, char *argv[]) {
    FILE* arqBin;
    int numBlocos;

    // Abrir arquivo binário desordenado
    arqBin = fopen("desordenado400.bin", "rb");
    if (arqBin == NULL) {
        printf("Erro ao abrir o arquivo binário.\n");
        return 1;
    }

    // Gerar os blocos ordenados (runs)
    criaRuns(arqBin, &numBlocos);

    // Realizar a intercalação balanceada
    intercalacaoBalanceada(numBlocos);

    // Imprimir os dados ordenados da fita final na tela
    imprimeFitaFinal();

    return 0;
}
