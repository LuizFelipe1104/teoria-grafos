#include <stdio.h>
#include <stdlib.h>
#include "grafo_lista.h"
 
GrafoLista *criar_grafo_lista(int n) {
    GrafoLista *grafo = malloc(sizeof(GrafoLista));
    grafo->n = n;
    grafo->adj = malloc(n * sizeof(No *));
 
    // cada lista começa vazia (ponteiro de cabeça = NULL)
    for (int i = 0; i < n; i++) {
        grafo->adj[i] = NULL;
    }
 
    return grafo;
}
 
// insere 'destino' no início da lista apontada por *cabeca
static void inserir_no(No **cabeca, int destino) {
    No *novo = malloc(sizeof(No));
    novo->destino = destino;
    novo->prox = *cabeca;
    *cabeca = novo;
}
 
// procura e remove o nó com valor 'destino' da lista apontada por *cabeca
static void remover_no(No **cabeca, int destino) {
    No *atual = *cabeca;
    No *anterior = NULL;
 
    while (atual != NULL) {
        if (atual->destino == destino) {
            if (anterior == NULL) {
                *cabeca = atual->prox; // removendo o primeiro nó
            } else {
                anterior->prox = atual->prox;
            }
            free(atual);
            return;
        }
        anterior = atual;
        atual = atual->prox;
    }
}
 
void inserir_aresta_lista(GrafoLista *grafo, int u, int v) {
    inserir_no(&grafo->adj[u], v);
    inserir_no(&grafo->adj[v], u); // grafo não-direcionado
}
 
void remover_aresta_lista(GrafoLista *grafo, int u, int v) {
    remover_no(&grafo->adj[u], v);
    remover_no(&grafo->adj[v], u);
}
 
int grau_lista(GrafoLista *grafo, int v) {
    int grau = 0;
    No *atual = grafo->adj[v];
    while (atual != NULL) {
        grau++;
        atual = atual->prox;
    }
    return grau;
}
 
int sao_adjacentes_lista(GrafoLista *grafo, int u, int v) {
    No *atual = grafo->adj[u];
    while (atual != NULL) {
        if (atual->destino == v) return 1;
        atual = atual->prox;
    }
    return 0;
}
 
void exibir_lista(GrafoLista *grafo) {
    for (int i = 0; i < grafo->n; i++) {
        printf("%d:", i);
        No *atual = grafo->adj[i];
        while (atual != NULL) {
            printf(" -> %d", atual->destino);
            atual = atual->prox;
        }
        printf("\n");
    }
}
 
void liberar_grafo_lista(GrafoLista *grafo) {
    for (int i = 0; i < grafo->n; i++) {
        No *atual = grafo->adj[i];
        while (atual != NULL) {
            No *tmp = atual;
            atual = atual->prox;
            free(tmp);
        }
    }
    free(grafo->adj);
    free(grafo);
}
 
