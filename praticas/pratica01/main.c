#include <stdio.h>
#include "grafo_matriz.h"
#include "grafo_lista.h"
 
int main() {
    int numero_vertices = 8;
 
    GrafoMatriz *matriz = criar_grafo_matriz(numero_vertices);
    GrafoLista *lista = criar_grafo_lista(numero_vertices);
 
    int arestas[][2] = {
        {0, 1}, {0, 2}, {0, 3},
        {1, 4}, {1, 5},
        {2, 3}, {2, 6},
        {3, 6},
        {7, 4}, {7, 5}, {7, 6}
    };
    int num_arestas = sizeof(arestas) / sizeof(arestas[0]);
 
    for (int i = 0; i < num_arestas; i++) {
        inserir_aresta_matriz(matriz, arestas[i][0], arestas[i][1]);
        inserir_aresta_lista(lista, arestas[i][0], arestas[i][1]);
    }
 
    printf("Matriz de Adjacencia\n");
    exibir_matriz(matriz);
 
    printf("\nLista de Adjacencia\n");
    exibir_lista(lista);
 
    printf("\nGrau do vertice 0 (matriz): %d\n", grau_matriz(matriz, 0));
    printf("Grau do vertice 0 (lista): %d\n", grau_lista(lista, 0));
 
    printf("0 e 1 sao adjacentes (matriz)? %d\n", sao_adjacentes_matriz(matriz, 0, 1));
    printf("0 e 1 sao adjacentes (lista)? %d\n", sao_adjacentes_lista(lista, 0, 1));
 
    remover_aresta_matriz(matriz, 0, 1);
    remover_aresta_lista(lista, 0, 1);
 
    printf("\nApos remover aresta 0-1:\n");
    printf("0 e 1 sao adjacentes (matriz)? %d\n", sao_adjacentes_matriz(matriz, 0, 1));
    printf("0 e 1 sao adjacentes (lista)? %d\n", sao_adjacentes_lista(lista, 0, 1));
 
    liberar_grafo_matriz(matriz);
    liberar_grafo_lista(lista);
 
    return 0;
}
 