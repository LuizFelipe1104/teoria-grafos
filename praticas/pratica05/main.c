#include <stdio.h>
#include <stdlib.h>
#include "grafo_lista.h"
#include "coloracao.h"
 
static void imprimir_coloracao(const char *rotulo, int *cor, int n, int num_cores) {
    printf("%s (usou %d cor(es)):\n", rotulo, num_cores);
    for (int i = 0; i < n; i++) {
        printf("  vertice %d -> cor %d\n", i, cor[i]);
    }
}
 
int main() {
    // ---- Grafo 1: o mesmo das praticas anteriores (8 vertices, com ciclos) ----
    int n1 = 8;
    GrafoLista *g1 = criar_grafo_lista(n1);
    int arestas1[][2] = {
        {0, 1}, {0, 2}, {0, 3},
        {1, 4}, {1, 5},
        {2, 3}, {2, 6},
        {3, 6},
        {7, 4}, {7, 5}, {7, 6}
    };
    for (int i = 0; i < 11; i++) {
        inserir_aresta_lista(g1, arestas1[i][0], arestas1[i][1]);
    }
 
    printf("=== Grafo 1 ===\n");
    exibir_lista(g1);
 
    int num_cores;
    int *cor_gulosa = coloracao_gulosa(g1, &num_cores);
    printf("\n");
    imprimir_coloracao("Coloracao gulosa (ordem natural)", cor_gulosa, n1, num_cores);
    free(cor_gulosa);
 
    int *cor_wp = coloracao_welsh_powell(g1, &num_cores);
    printf("\n");
    imprimir_coloracao("Coloracao Welsh-Powell", cor_wp, n1, num_cores);
    free(cor_wp);
 
    printf("\nO grafo 1 e bipartido? %s\n", eh_bipartido(g1) ? "Sim" : "Nao");
 
    liberar_grafo_lista(g1);
 
    // ---- Grafo 2: "crown graph" bipartido, mas numerado de propósito para
    // confundir a ordem natural (a1, b1, a2, b2, a3, b3 intercalados) ----
    // Partição A = {0, 2, 4}, Partição B = {1, 3, 5}
    // Arestas: a1-b2, a1-b3, a2-b1, a2-b3, a3-b1, a3-b2  (falta so a_i-b_i)
    printf("\n=== Grafo 2 (crown graph, numeracao intercalada de proposito) ===\n");
    int n2 = 6;
    GrafoLista *g2 = criar_grafo_lista(n2);
    inserir_aresta_lista(g2, 0, 3); // a1-b2
    inserir_aresta_lista(g2, 0, 5); // a1-b3
    inserir_aresta_lista(g2, 2, 1); // a2-b1
    inserir_aresta_lista(g2, 2, 5); // a2-b3
    inserir_aresta_lista(g2, 4, 1); // a3-b1
    inserir_aresta_lista(g2, 4, 3); // a3-b2
 
    exibir_lista(g2);
 
    cor_gulosa = coloracao_gulosa(g2, &num_cores);
    printf("\n");
    imprimir_coloracao("Coloracao gulosa (ordem natural)", cor_gulosa, n2, num_cores);
    free(cor_gulosa);
 
    cor_wp = coloracao_welsh_powell(g2, &num_cores);
    printf("\n");
    imprimir_coloracao("Coloracao Welsh-Powell", cor_wp, n2, num_cores);
    free(cor_wp);
 
    int bipartido2 = eh_bipartido(g2);
    printf("\nO grafo 2 e bipartido? %s\n", bipartido2 ? "Sim" : "Nao");
    if (bipartido2) {
        printf("-> Numero cromatico real e 2, mesmo que as heuristicas acima tenham\n");
        printf("   usado mais cores por causa da ordem de processamento. eh_bipartido()\n");
        printf("   da uma resposta EXATA aqui; coloracao_gulosa/welsh_powell sao so\n");
        printf("   aproximacoes (o problema geral de k-coloracao e NP-dificil).\n");
    }
 
    liberar_grafo_lista(g2);
 
    return 0;
}