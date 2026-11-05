#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "grafo_lista.h"
#include "cobertura.h"

// Gerador pseudoaleatório próprio (em vez de rand()) para que o experimento
// gere EXATAMENTE os mesmos grafos em qualquer computador/sistema.
static unsigned int semente = 12345u;

static int proximo_aleatorio(void) {
    semente = semente * 1664525u + 1013904223u;
    return (int)((semente >> 16) % 100u); // 0..99
}

// Cada par de vértices vira aresta com probabilidade 'percentual'%
static GrafoLista *gerar_grafo_aleatorio(int n, int percentual, int *num_arestas) {
    GrafoLista *g = criar_grafo_lista(n);
    int m = 0;
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            if (proximo_aleatorio() < percentual) {
                inserir_aresta_lista(g, i, j);
                m++;
            }
        }
    }
    *num_arestas = m;
    return g;
}

static void imprimir_cobertura(const char *rotulo, int *cobertura, int n, int tamanho) {
    printf("%s (tamanho %d): {", rotulo, tamanho);
    int primeiro = 1;
    for (int i = 0; i < n; i++) {
        if (cobertura[i]) {
            printf("%s%d", primeiro ? "" : ", ", i);
            primeiro = 0;
        }
    }
    printf("}\n");
}

// Tempo medio (em microssegundos) de uma execucao: repete ate acumular pelo
// menos ~50 ms, porque uma unica execucao e rapida demais para o relogio medir.
static double medir_bt(GrafoLista *g, int *cobertura) {
    clock_t inicio = clock();
    long repeticoes = 0;
    do {
        cobertura_minima_bt(g, cobertura, NULL);
        repeticoes++;
    } while (clock() - inicio < CLOCKS_PER_SEC / 20);
    double segundos = (double)(clock() - inicio) / CLOCKS_PER_SEC;
    return segundos / repeticoes * 1e6;
}

static double medir_gulosa(GrafoLista *g, int *cobertura) {
    clock_t inicio = clock();
    long repeticoes = 0;
    do {
        cobertura_gulosa(g, cobertura);
        repeticoes++;
    } while (clock() - inicio < CLOCKS_PER_SEC / 20);
    double segundos = (double)(clock() - inicio) / CLOCKS_PER_SEC;
    return segundos / repeticoes * 1e6;
}

int main() {
    // ================= Parte 1: um caso em que o guloso erra =================
    // 6 vertices, 7 arestas. Graus: 0 e 3 tem grau 3; os demais tem grau 2.
    // O guloso comeca pelo vertice 0 (maior grau) e acaba usando 4 vertices,
    // mas {1, 2, 3} ja cobre tudo com apenas 3.
    printf("=== Parte 1: comparando as duas abordagens num grafo pequeno ===\n");
    int n1 = 6;
    int arestas1[][2] = {{0, 1}, {0, 2}, {0, 3}, {1, 5}, {2, 4}, {3, 4}, {3, 5}};
    GrafoLista *g1 = criar_grafo_lista(n1);
    for (int i = 0; i < 7; i++) {
        inserir_aresta_lista(g1, arestas1[i][0], arestas1[i][1]);
    }

    int cob_bt[6], cob_gu[6];
    long nos;
    int tam_bt = cobertura_minima_bt(g1, cob_bt, &nos);
    int tam_gu = cobertura_gulosa(g1, cob_gu);

    imprimir_cobertura("Backtracking (otima)", cob_bt, n1, tam_bt);
    imprimir_cobertura("Gulosa", cob_gu, n1, tam_gu);
    printf("Backtracking e cobertura valida? %s\n", eh_cobertura(g1, cob_bt) ? "Sim" : "Nao");
    printf("Gulosa e cobertura valida?       %s\n", eh_cobertura(g1, cob_gu) ? "Sim" : "Nao");

    // eh_cobertura tambem serve para reprovar um conjunto que NAO cobre tudo
    int incompleta[6] = {1, 0, 0, 0, 0, 0}; // so o vertice 0
    printf("{0} sozinho e cobertura valida?  %s\n", eh_cobertura(g1, incompleta) ? "Sim" : "Nao");
    printf("-> A gulosa usou %d vertice(s) a mais que o minimo.\n\n", tam_gu - tam_bt);
    liberar_grafo_lista(g1);

    // ================= Parte 2: experimento de tempo =================
    printf("=== Parte 2: experimento de tempo (grafos aleatorios, ~30%% das arestas possiveis) ===\n");
    printf("Tempos em microssegundos (media de varias execucoes). 'nos' = chamadas recursivas do backtracking.\n\n");
    printf("%3s %5s | %6s %7s | %8s %10s | %10s %10s | %9s\n",
           "n", "arest", "otimo", "guloso", "nos BT", "tempo BT", "tempo gul.", "BT/gul.", "valida?");
    printf("------------------------------------------------------------------------------------------\n");

    int tamanhos[] = {5, 8, 10, 12, 15};
    int quantidade = sizeof(tamanhos) / sizeof(tamanhos[0]);

    for (int k = 0; k < quantidade; k++) {
        int n = tamanhos[k];
        int m;
        GrafoLista *g = gerar_grafo_aleatorio(n, 30, &m);

        int *cb = malloc(n * sizeof(int));
        int *cg = malloc(n * sizeof(int));

        long nos_bt;
        int otimo = cobertura_minima_bt(g, cb, &nos_bt);
        int guloso = cobertura_gulosa(g, cg);
        int valido = eh_cobertura(g, cb) && eh_cobertura(g, cg);

        double t_bt = medir_bt(g, cb);
        double t_gu = medir_gulosa(g, cg);

        printf("%3d %5d | %6d %7d | %8ld %10.2f | %10.2f %9.1fx | %9s\n",
               n, m, otimo, guloso, nos_bt, t_bt, t_gu, t_bt / t_gu, valido ? "sim" : "NAO");

        free(cb);
        free(cg);
        liberar_grafo_lista(g);
    }

    return 0;
}