#include <stdio.h>
#include <stdlib.h>
#include "grafo_matriz.h"
 
GrafoMatriz *criar_grafo_matriz(int n) {
    GrafoMatriz *grafo = malloc(sizeof(GrafoMatriz));
    grafo->n = n;
 
    // aloca um array de n ponteiros (uma "linha" para cada vértice)
    grafo->adj = malloc(n * sizeof(int *));
 
    // para cada linha, aloca n inteiros e já zera tudo com calloc
    for (int i = 0; i < n; i++) {
        grafo->adj[i] = calloc(n, sizeof(int));
    }
 
    return grafo;
}
 
void inserir_aresta_matriz(GrafoMatriz *grafo, int u, int v) {
    grafo->adj[u][v] = 1;
    grafo->adj[v][u] = 1; // grafo não-direcionado: marca os dois sentidos
}
 
void remover_aresta_matriz(GrafoMatriz *grafo, int u, int v) {
    grafo->adj[u][v] = 0;
    grafo->adj[v][u] = 0;
}
 
int grau_matriz(GrafoMatriz *grafo, int v) {
    int grau = 0;
    for (int i = 0; i < grafo->n; i++) {
        grau += grafo->adj[v][i];
    }
    return grau;
}
 
int sao_adjacentes_matriz(GrafoMatriz *grafo, int u, int v) {
    return grafo->adj[u][v] != 0;
}
 
void exibir_matriz(GrafoMatriz *grafo) {
    for (int i = 0; i < grafo->n; i++) {
        for (int j = 0; j < grafo->n; j++) {
            printf("%3d", grafo->adj[i][j]);
        }
        printf("\n");
    }
}
 
void liberar_grafo_matriz(GrafoMatriz *grafo) {
    for (int i = 0; i < grafo->n; i++) {
        free(grafo->adj[i]); // libera cada linha
    }
    free(grafo->adj);        // libera o array de ponteiros
    free(grafo);              // libera a struct em si
}