#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define TAMANHO_VETOR 100000000

// Função para registrar os dados no arquivo CSV
void registrar_csv(FILE *arquivo, const char *otimizacao, const char *laco, double tempo) {
    if (arquivo) {
        fprintf(arquivo, "%s,%s,%f\n", otimizacao, laco, tempo);
    }
}

int main(int argc, char *argv[]) {
    // --- Validação e Configuração do CSV ---
    if (argc < 2) {
        fprintf(stderr, "Uso: %s <Nivel_Otimizacao>\nExemplo: %s O2\n", argv[0], argv[0]);
        return 1;
    }
    char *nivel_otimizacao = argv[1];

    FILE *arquivo_csv = fopen("dados_desempenho.csv", "a");
    if (arquivo_csv == NULL) {
        fprintf(stderr, "Erro ao abrir o arquivo CSV para escrita.\n");
        return 1;
    }
    
    // Aloca um vetor de inteiros de forma dinâmica
    int *vetor = (int *)malloc(TAMANHO_VETOR * sizeof(int));
    if (vetor == NULL) {
        fprintf(stderr, "Falha na alocacao de memoria\n");
        fclose(arquivo_csv);
        return 1;
    }

    struct timespec inicio, fim;
    double tempo_gasto;

    // --- Laço 1: Inicialização do Vetor (Alto ILP) ---
    clock_gettime(CLOCK_MONOTONIC, &inicio);
    for (int i = 0; i < TAMANHO_VETOR; i++) {
        vetor[i] = (i % 10) + 1;
    }
    clock_gettime(CLOCK_MONOTONIC, &fim);
    tempo_gasto = (fim.tv_sec - inicio.tv_sec) + (fim.tv_nsec - inicio.tv_nsec) / 1e9;
    printf("Laço 1 (Inicializacao).................: %f segundos\n", tempo_gasto);
    registrar_csv(arquivo_csv, nivel_otimizacao, "Laco_1_Inicializacao", tempo_gasto);

    // --- Laço 2: Soma Acumulativa (Dependência de Dados) ---
    long long soma_dependente = 0;
    clock_gettime(CLOCK_MONOTONIC, &inicio);
    for (int i = 0; i < TAMANHO_VETOR; i++) {
        soma_dependente += vetor[i];
    }
    clock_gettime(CLOCK_MONOTONIC, &fim);
    tempo_gasto = (fim.tv_sec - inicio.tv_sec) + (fim.tv_nsec - inicio.tv_nsec) / 1e9;
    printf("Laço 2 (Soma com Dependencia)..........: %f segundos\n", tempo_gasto);
    registrar_csv(arquivo_csv, nivel_otimizacao, "Laco_2_Dependente", tempo_gasto);

    // --- Laço 3: Quebra de Dependências (Fator 4) ---
    long long s1=0, s2=0, s3=0, s4=0;
    clock_gettime(CLOCK_MONOTONIC, &inicio);
    for (int i = 0; i < TAMANHO_VETOR; i += 4) {
        s1 += vetor[i];
        s2 += vetor[i+1];
        s3 += vetor[i+2];
        s4 += vetor[i+3];
    }
    long long soma_total_indep4 = s1 + s2 + s3 + s4;
    clock_gettime(CLOCK_MONOTONIC, &fim);
    tempo_gasto = (fim.tv_sec - inicio.tv_sec) + (fim.tv_nsec - inicio.tv_nsec) / 1e9;
    printf("Laço 3 (Quebra de Dependencia, Fator 4): %f segundos\n", tempo_gasto);
    registrar_csv(arquivo_csv, nivel_otimizacao, "Laco_3_Fator_4", tempo_gasto);

    // --- Laço 4: Quebra de Dependências (Fator 8) ---
    long long a1=0, a2=0, a3=0, a4=0, a5=0, a6=0, a7=0, a8=0;
    clock_gettime(CLOCK_MONOTONIC, &inicio);
    for (int i = 0; i < TAMANHO_VETOR; i += 8) {
        a1 += vetor[i];   a2 += vetor[i+1];
        a3 += vetor[i+2]; a4 += vetor[i+3];
        a5 += vetor[i+4]; a6 += vetor[i+5];
        a7 += vetor[i+6]; a8 += vetor[i+7];
    }
    long long soma_total_indep8 = a1+a2+a3+a4+a5+a6+a7+a8;
    clock_gettime(CLOCK_MONOTONIC, &fim);
    tempo_gasto = (fim.tv_sec - inicio.tv_sec) + (fim.tv_nsec - inicio.tv_nsec) / 1e9;
    printf("Laço 4 (Quebra de Dependencia, Fator 8): %f segundos\n", tempo_gasto);
    registrar_csv(arquivo_csv, nivel_otimizacao, "Laco_4_Fator_8", tempo_gasto);

    printf("\nResultados das somas (para verificacao):\n");
    printf("Dependente: %lld | Fator 4: %lld | Fator 8: %lld\n", soma_dependente, soma_total_indep4, soma_total_indep8);
    
    free(vetor);
    fclose(arquivo_csv);
    return 0;
}