#include <stdio.h>
#include <stdlib.h>
#include "grafo_lista.h"
#include "busca_largura.h"
#include "busca_profundidade.h"
 
int main() {
    int numero_vertices = 8;
    GrafoLista *g = criar_grafo_lista(numero_vertices);
 
    int arestas[][2] = {
        {0, 1}, {0, 2}, {0, 3},
        {1, 4}, {1, 5},
        {2, 3}, {2, 6},
        {3, 6},
        {7, 4}, {7, 5}, {7, 6}
    };
    int num_arestas = sizeof(arestas) / sizeof(arestas[0]);
 
    for (int i = 0; i < num_arestas; i++) {
        inserir_aresta_lista(g, arestas[i][0], arestas[i][1]);
    }
 
    printf("Lista de Adjacencia\n");
    exibir_lista(g);
 
    // ---- BFS ----
    int *dist = malloc(numero_vertices * sizeof(int));
    int *pred = malloc(numero_vertices * sizeof(int));
    bfs(g, 0, dist, pred);
 
    printf("\nBFS a partir do vertice 0\n");
    for (int i = 0; i < numero_vertices; i++) {
        printf("Vertice %d: distancia = %d, predecessor = %d\n", i, dist[i], pred[i]);
    }
    free(dist);
    free(pred);
 
    // ---- DFS com tempos de entrada/saida ----
    int *visitado = calloc(numero_vertices, sizeof(int));
    int *entrada = malloc(numero_vertices * sizeof(int));
    int *saida = malloc(numero_vertices * sizeof(int));
    int tempo = 0;
    dfs_recursiva(g, 0, visitado, &tempo, entrada, saida);
 
    printf("\nDFS a partir do vertice 0\n");
    for (int i = 0; i < numero_vertices; i++) {
        if (visitado[i]) {
            printf("Vertice %d: entrada = %d, saida = %d\n", i, entrada[i], saida[i]);
        }
    }
    free(visitado);
    free(entrada);
    free(saida);
 
    // ---- Componentes, ciclo e bipartição ----
    printf("\nNumero de componentes conexos: %d\n", contar_componentes(g));
    printf("O grafo tem ciclo? %s\n", tem_ciclo(g) ? "Sim" : "Nao");
    printf("O grafo e bipartido? %s\n", eh_bipartido(g) ? "Sim" : "Nao");
 
    liberar_grafo_lista(g);
    return 0;
}
 