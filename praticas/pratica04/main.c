#include <stdio.h>
#include <stdlib.h>
#include "grafo_lista.h"
#include "conectividade.h"
#include "planaridade.h"
 
static void construir_grafo_articulacoes(GrafoLista *g) {
    // Dois "triangulos" (0-1-2 e 3-4-5) ligados por uma ponte (2-3), mais uma
    // folha (6) pendurada em 3 por outra ponte.
    // Vertices de corte esperados: 2 e 3. Pontes esperadas: (2,3) e (3,6).
    inserir_aresta_lista(g, 0, 1);
    inserir_aresta_lista(g, 1, 2);
    inserir_aresta_lista(g, 2, 0);
    inserir_aresta_lista(g, 2, 3); // ponte
    inserir_aresta_lista(g, 3, 4);
    inserir_aresta_lista(g, 4, 5);
    inserir_aresta_lista(g, 5, 3);
    inserir_aresta_lista(g, 3, 6); // ponte
}
 
static void testar_planaridade(const char *nome, GrafoLista *g) {
    printf("\n--- %s ---\n", nome);
    printf("Satisfaz Euler (m <= 3n-6)? %s\n", eh_planar_euler(g) ? "Sim" : "Nao");
    printf("Contem K5 como subgrafo?    %s\n", contem_k5_subgrafo(g) ? "Sim" : "Nao");
    printf("Contem K3,3 como subgrafo?  %s\n", contem_k33_subgrafo(g) ? "Sim" : "Nao");
    printf("Classificacao (heuristica): %s\n", eh_planar_heuristica(g) ? "Planar" : "Nao-planar");
}
 
int main() {
    // ---- Parte 1: articulacoes e pontes ----
    GrafoLista *g1 = criar_grafo_lista(7);
    construir_grafo_articulacoes(g1);
 
    printf("=== Articulacoes e pontes ===\n");
    exibir_lista(g1);
 
    int quantidade;
    int *articulacoes = encontrar_articulacoes(g1, &quantidade);
    printf("\nVertices de articulacao (%d): ", quantidade);
    for (int i = 0; i < quantidade; i++) printf("%d ", articulacoes[i]);
    printf("\n");
    free(articulacoes);
 
    Aresta *pontes = detectar_pontes(g1, &quantidade);
    printf("Pontes (%d): ", quantidade);
    for (int i = 0; i < quantidade; i++) printf("(%d-%d) ", pontes[i].u, pontes[i].v);
    printf("\n");
    free(pontes);
 
    liberar_grafo_lista(g1);
 
    // ---- Parte 2: planaridade ----
    // Grafo planar simples: ciclo de 4 vertices (quadrado)
    GrafoLista *quadrado = criar_grafo_lista(4);
    inserir_aresta_lista(quadrado, 0, 1);
    inserir_aresta_lista(quadrado, 1, 2);
    inserir_aresta_lista(quadrado, 2, 3);
    inserir_aresta_lista(quadrado, 3, 0);
    testar_planaridade("Ciclo de 4 vertices (planar)", quadrado);
    liberar_grafo_lista(quadrado);
 
    // K5: grafo completo de 5 vertices, classico exemplo nao-planar
    GrafoLista *k5 = criar_grafo_lista(5);
    for (int i = 0; i < 5; i++) {
        for (int j = i + 1; j < 5; j++) {
            inserir_aresta_lista(k5, i, j);
        }
    }
    testar_planaridade("K5 (nao-planar, falha ate em Euler)", k5);
    liberar_grafo_lista(k5);
 
    // K3,3: bipartido completo 3x3, nao-planar mas SATISFAZ Euler
    // (mostra na pratica por que Euler sozinho nao basta)
    GrafoLista *k33 = criar_grafo_lista(6);
    for (int i = 0; i < 3; i++) {
        for (int j = 3; j < 6; j++) {
            inserir_aresta_lista(k33, i, j);
        }
    }
    testar_planaridade("K3,3 (nao-planar, mas passa em Euler)", k33);
    liberar_grafo_lista(k33);
 
    return 0;
}