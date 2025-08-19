#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Define um tamanho grande para o vetor para que os tempos de execução sejam mensuráveis
#define TAMANHO_VETOR 100000000

int main() {
    // Aloca um vetor de inteiros de forma dinâmica
    int *vetor = (int *)malloc(TAMANHO_VETOR * sizeof(int));
    if (vetor == NULL) {
        fprintf(stderr, "Falha na alocacao de memoria\n");
        return 1;
    }

    struct timespec inicio, fim;
    double tempo_gasto;

    // --- Laço 1: Inicialstartização do Vetor (Alto ILP) ---
    // Este laço não possui dependências entre as iterações. Cada cálculo é independente.

    for (int i = 0; i < TAMANHO_VETOR; i++) {
        vetor[i] = (i % 10) + 1; // Cálculo simples para inicializar
    }
    

    // --- Laço 2: Soma Acumulativa (Dependência de Dados) ---
    // A variável 'soma_dependente' cria uma dependência entre cada iteração.
    // A iteração 'i' não pode começar antes que a iteração 'i-1' termine de atualizar a soma.
    long long soma_dependente = 0;
    clock_gettime(CLOCK_MONOTONIC, &inicio);
    for (int i = 0; i < TAMANHO_VETOR; i++) {
        soma_dependente += vetor[i];
    }
    clock_gettime(CLOCK_MONOTONIC, &fim);
    tempo_gasto = (fim.tv_sec - inicio.tv_sec) + (fim.tv_nsec - inicio.tv_nsec) / 1e9;
    printf("Laco 2 (Soma com Dependencia)..........: %f segundos\n", tempo_gasto);

    // --- Laço 3: Quebra de Dependências com Múltiplos Acumuladores ---
    // Utilizamos quatro acumuladores para quebrar a dependência. As quatro somas dentro do laço
    // podem ser executadas em paralelo pelo processador.
    long long soma_indep1 = 0, soma_indep2 = 0, soma_indep3 = 0, soma_indep4 = 0, soma_indep5 = 0;
    clock_gettime(CLOCK_MONOTONIC, &inicio);
    for (int i = 0; i < TAMANHO_VETOR; i += 5) {
        soma_indep1 += vetor[i];
        soma_indep2 += vetor[i+1];
        soma_indep3 += vetor[i+2];
        soma_indep4 += vetor[i+3];
        soma_indep5 += vetor[i+4];
       
    }
    long long soma_total_independente = soma_indep1 + soma_indep2 + soma_indep3 + soma_indep4 + soma_indep5;
    clock_gettime(CLOCK_MONOTONIC, &fim);
    tempo_gasto = (fim.tv_sec - inicio.tv_sec) + (fim.tv_nsec - inicio.tv_nsec) / 1e9;
    printf("Laco 3 (Soma com Quebra de Dependencia): %f segundos\n", tempo_gasto);

    // Imprime os resultados para garantir que os cálculos estão corretos
    printf("\nResultado da Soma (Dependente):         %lld\n", soma_dependente);
    printf("Resultado da Soma (Independente):       %lld\n", soma_total_independente);

    free(vetor);
    return 0;
}