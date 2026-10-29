#include <stdio.h>
#include <stdlib.h>
#include "agm.h"

static void imprimir_mst(const char *rotulo, Aresta *mst, int quantidade, int peso_total) {
    printf("%s (%d aresta(s), peso total = %d):\n", rotulo, quantidade, peso_total);
    for (int i = 0; i < quantidade; i++) {
        printf("  %d - %d (peso %d)\n", mst[i].u, mst[i].v, mst[i].peso);
    }
}

static void comparar(const char *titulo, GrafoListaPonderado *g, int origem_prim) {
    int n = g->n;
    int m;
    Aresta *arestas = extrair_arestas(g, &m);
    Aresta *mst = malloc(n * sizeof(Aresta));
    int peso;

    printf("=== %s ===\n", titulo);
    printf("%d vertices, %d arestas\n\n", n, m);

    int q = kruskal(n, arestas, m, mst, &peso);
    imprimir_mst("Kruskal", mst, q, peso);

    printf("\n");
    q = prim(g, origem_prim, mst, &peso);
    imprimir_mst("Prim", mst, q, peso);
    printf("\n");

    free(arestas);
    free(mst);
}

int main() {
    // ---- Grafo 1: conexo, 9 vertices (0..8) ----
    GrafoListaPonderado *g1 = criar_grafo_ponderado(9);
    inserir_aresta_ponderada(g1, 0, 1, 4);
    inserir_aresta_ponderada(g1, 0, 7, 8);
    inserir_aresta_ponderada(g1, 1, 2, 8);
    inserir_aresta_ponderada(g1, 1, 7, 11);
    inserir_aresta_ponderada(g1, 2, 3, 7);
    inserir_aresta_ponderada(g1, 2, 5, 4);
    inserir_aresta_ponderada(g1, 2, 8, 2);
    inserir_aresta_ponderada(g1, 3, 4, 9);
    inserir_aresta_ponderada(g1, 3, 5, 14);
    inserir_aresta_ponderada(g1, 4, 5, 10);
    inserir_aresta_ponderada(g1, 5, 6, 2);
    inserir_aresta_ponderada(g1, 6, 7, 1);
    inserir_aresta_ponderada(g1, 6, 8, 6);
    inserir_aresta_ponderada(g1, 7, 8, 7);
    comparar("Grafo 1 (conexo)", g1, 0);
    liberar_grafo_ponderado(g1);

    // ---- Grafo 2: DESCONEXO, dois triangulos separados ----
    // Componente A = {0,1,2}, componente B = {3,4,5}
    GrafoListaPonderado *g2 = criar_grafo_ponderado(6);
    inserir_aresta_ponderada(g2, 0, 1, 1);
    inserir_aresta_ponderada(g2, 1, 2, 2);
    inserir_aresta_ponderada(g2, 0, 2, 3);
    inserir_aresta_ponderada(g2, 3, 4, 5);
    inserir_aresta_ponderada(g2, 4, 5, 4);
    inserir_aresta_ponderada(g2, 3, 5, 6);
    comparar("Grafo 2 (desconexo: Kruskal acha floresta, Prim so cobre a origem)", g2, 0);
    liberar_grafo_ponderado(g2);

    return 0;
}