#include <stdio.h>
#include <stdlib.h>
#include "dijkstra.h"

static void imprimir_caminho(int origem, int destino, int *pred, int *dist) {
    int *caminho = reconstruir_caminho(origem, destino, pred);

    printf("  %d -> %d: ", origem, destino);
    if (caminho == NULL) {
        printf("sem caminho (inalcancavel)\n");
        return;
    }

    for (int i = 0; caminho[i] != -1; i++) {
        if (i > 0) printf(" -> ");
        printf("%d", caminho[i]);
    }
    printf("  (custo %d)\n", dist[destino]);
    free(caminho);
}

int main() {
    // Grafo de exemplo (7 vertices; o vertice 6 fica isolado de proposito
    // para testar o caso "sem caminho"):
    //
    //   0-1:7   0-2:9   0-5:14
    //   1-2:10  1-3:15
    //   2-3:11  2-5:2
    //   3-4:6
    //   4-5:9
    int n = 7;
    GrafoListaPonderado *g = criar_grafo_ponderado(n);

    inserir_aresta_ponderada(g, 0, 1, 7);
    inserir_aresta_ponderada(g, 0, 2, 9);
    inserir_aresta_ponderada(g, 0, 5, 14);
    inserir_aresta_ponderada(g, 1, 2, 10);
    inserir_aresta_ponderada(g, 1, 3, 15);
    inserir_aresta_ponderada(g, 2, 3, 11);
    inserir_aresta_ponderada(g, 2, 5, 2);
    inserir_aresta_ponderada(g, 3, 4, 6);
    inserir_aresta_ponderada(g, 4, 5, 9);

    int *dist = malloc(n * sizeof(int));
    int *pred = malloc(n * sizeof(int));

    int origem = 0;
    dijkstra(g, origem, dist, pred);

    printf("Dijkstra a partir do vertice %d\n\n", origem);
    printf("Distancias e predecessores:\n");
    for (int i = 0; i < n; i++) {
        if (dist[i] == INFINITO) {
            printf("  vertice %d: distancia = INF, predecessor = %d\n", i, pred[i]);
        } else {
            printf("  vertice %d: distancia = %d, predecessor = %d\n", i, dist[i], pred[i]);
        }
    }

    printf("\nCaminhos minimos:\n");
    for (int destino = 0; destino < n; destino++) {
        imprimir_caminho(origem, destino, pred, dist);
    }

    free(dist);
    free(pred);
    liberar_grafo_ponderado(g);

    return 0;
}