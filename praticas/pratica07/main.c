#include <stdio.h>
#include <stdlib.h>
#include "bellman_ford.h"

static void testar(const char *titulo, GrafoListaPonderado *g, int origem) {
    int n = g->n;
    int *dist = malloc(n * sizeof(int));
    int *pred = malloc(n * sizeof(int));

    printf("=== %s ===\n", titulo);
    printf("Origem: %d\n", origem);

    if (bellman_ford(g, origem, dist, pred)) {
        printf("Sem ciclo negativo alcancavel. Distancias e predecessores:\n");
        for (int i = 0; i < n; i++) {
            if (dist[i] == INFINITO) {
                printf("  vertice %d: distancia = INF, predecessor = %d\n", i, pred[i]);
            } else {
                printf("  vertice %d: distancia = %d, predecessor = %d\n", i, dist[i], pred[i]);
            }
        }
    } else {
        printf("bellman_ford retornou 0: existe ciclo de custo negativo alcancavel!\n");
        printf("(as distancias nao sao confiaveis nesse caso)\n");
        reconstruir_ciclo_negativo(g, dist, pred);
    }
    printf("\n");

    free(dist);
    free(pred);
}

int main() {
    // ---- Grafo 1: pesos negativos, mas SEM ciclo negativo ----
    // (6 vertices; o vertice 5 e isolado de proposito para mostrar INF)
    GrafoListaPonderado *g1 = criar_grafo_ponderado(6);
    inserir_aresta_ponderada(g1, 0, 1, 6);
    inserir_aresta_ponderada(g1, 0, 2, 7);
    inserir_aresta_ponderada(g1, 1, 2, 8);
    inserir_aresta_ponderada(g1, 1, 3, 5);
    inserir_aresta_ponderada(g1, 1, 4, -4);
    inserir_aresta_ponderada(g1, 2, 3, -3);
    inserir_aresta_ponderada(g1, 2, 4, 9);
    inserir_aresta_ponderada(g1, 3, 1, -2);
    inserir_aresta_ponderada(g1, 4, 0, 2);
    inserir_aresta_ponderada(g1, 4, 3, 7);
    testar("Grafo 1 (pesos negativos, sem ciclo negativo)", g1, 0);
    liberar_grafo_ponderado(g1);

    // ---- Grafo 2: ciclo negativo 1 -> 2 -> 3 -> 1 (custo -1 + -1 + -1 = -3) ----
    // O vertice 4 pendura no ciclo, para o retrocesso de n passos ter utilidade
    GrafoListaPonderado *g2 = criar_grafo_ponderado(5);
    inserir_aresta_ponderada(g2, 0, 1, 1);
    inserir_aresta_ponderada(g2, 1, 2, -1);
    inserir_aresta_ponderada(g2, 2, 3, -1);
    inserir_aresta_ponderada(g2, 3, 1, -1);
    inserir_aresta_ponderada(g2, 3, 4, 2);
    testar("Grafo 2 (com ciclo negativo alcancavel)", g2, 0);
    liberar_grafo_ponderado(g2);

    // ---- Grafo 3: ciclo negativo 2 <-> 3, mas INALCANCAVEL a partir do 0 ----
    GrafoListaPonderado *g3 = criar_grafo_ponderado(4);
    inserir_aresta_ponderada(g3, 0, 1, 1);
    inserir_aresta_ponderada(g3, 2, 3, -1);
    inserir_aresta_ponderada(g3, 3, 2, -1);
    testar("Grafo 3 (ciclo negativo existe, mas nao e alcancavel da origem)", g3, 0);
    printf("-> O Bellman-Ford so enxerga ciclos negativos ALCANCAVEIS da origem.\n");
    liberar_grafo_ponderado(g3);

    return 0;
}