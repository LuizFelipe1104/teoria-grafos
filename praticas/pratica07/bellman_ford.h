#ifndef BELLMAN_FORD_H
#define BELLMAN_FORD_H

#include <limits.h>

#define INFINITO INT_MAX

// ---------- Grafo DIRECIONADO com lista de adjacência ponderada ----------

typedef struct NoPonderado {
    int destino;
    int peso;               // pode ser negativo
    struct NoPonderado *prox;
} NoPonderado;

typedef struct {
    int n;                  // número de vértices
    NoPonderado **adj;      // adj[u] = lista de arestas que SAEM de u
} GrafoListaPonderado;

GrafoListaPonderado *criar_grafo_ponderado(int n);
// Insere apenas a aresta u -> v (direcionada). Diferente da prática 06,
// aqui NÃO inserimos o sentido contrário: uma aresta não-direcionada com
// peso negativo já seria, por si só, um ciclo negativo (u -> v -> u).
void inserir_aresta_ponderada(GrafoListaPonderado *g, int u, int v, int peso);
void liberar_grafo_ponderado(GrafoListaPonderado *g);

// dist e pred devem apontar para arrays já alocados com g->n posições.
// Retorna 1 se terminou com sucesso (dist/pred são os caminhos mínimos) ou
// 0 se existe um ciclo de custo negativo ALCANÇÁVEL a partir de origem
// (nesse caso dist/pred não representam caminhos mínimos válidos).
// dist[v] = INFINITO e pred[v] = -1 quando v é inalcançável.
int bellman_ford(GrafoListaPonderado *g, int origem, int *dist, int *pred);

// Chamar logo após bellman_ford ter retornado 0. Encontra um ciclo negativo,
// IMPRIME o ciclo (com o custo total) e retorna quantos vértices distintos
// ele tem. Retorna 0 (sem imprimir nada) se não houver ciclo negativo.
// Não altera os arrays dist e pred recebidos.
int reconstruir_ciclo_negativo(GrafoListaPonderado *g, int *dist, int *pred);

#endif