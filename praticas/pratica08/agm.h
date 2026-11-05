#ifndef AGM_H
#define AGM_H

#include <limits.h>

#define INFINITO INT_MAX

// ---------- Grafo NÃO-direcionado com lista de adjacência ponderada ----------

typedef struct NoPonderado {
    int destino;
    int peso;
    struct NoPonderado *prox;
} NoPonderado;

typedef struct {
    int n;                  // número de vértices
    NoPonderado **adj;
} GrafoListaPonderado;

typedef struct {
    int u, v, peso;
} Aresta;

GrafoListaPonderado *criar_grafo_ponderado(int n);
// Insere a aresta nos DOIS sentidos (u -> v e v -> u) com o mesmo peso.
void inserir_aresta_ponderada(GrafoListaPonderado *g, int u, int v, int peso);
void liberar_grafo_ponderado(GrafoListaPonderado *g);

// Devolve um array (alocado, o caller dá free) com cada aresta do grafo UMA
// única vez, e coloca em *m a quantidade. Laços (u == v) são ignorados,
// pois nunca fazem parte de uma árvore geradora.
Aresta *extrair_arestas(GrafoListaPonderado *g, int *m);

// ---------- Union-Find (DSU) ----------

typedef struct {
    int *pai, *rank;
    int n;
} DSU;

DSU *criar_dsu(int n);
void liberar_dsu(DSU *d);
// Retorna o representante (raiz) do conjunto de x, com COMPRESSÃO DE CAMINHO.
int find(DSU *d, int x);
// Une os conjuntos de x e y por RANK. Retorna 1 se uniu, 0 se já estavam
// no mesmo conjunto (ou seja, a aresta x-y fecharia um ciclo).
int unir(DSU *d, int x, int y);

// ---------- Min-Heap (usado pelo Prim) ----------

typedef struct {
    int vertice;
    int distancia;   // aqui significa "chave": peso da melhor aresta até a árvore
} NoHeap;

typedef struct {
    NoHeap *dados;
    int capacidade, tamanho;
    int *posicao;    // mapeia vértice -> índice no heap (-1 se não está no heap)
} MinHeap;

MinHeap *criar_min_heap(int capacidade);
int heap_vazio(MinHeap *h);
int heap_contem(MinHeap *h, int vertice);
void inserir_heap(MinHeap *h, int vertice, int distancia);
NoHeap extrair_minimo(MinHeap *h);
void diminuir_chave(MinHeap *h, int vertice, int nova_distancia);
void liberar_min_heap(MinHeap *h);

// ---------- Árvores geradoras mínimas ----------

// Kruskal: recebe a lista de arestas (n vértices, m arestas) e preenche mst
// (capacidade mínima n) com as arestas escolhidas. Não altera o vetor
// 'arestas' (ordena uma cópia). Retorna quantas arestas entraram na AGM:
// n-1 se o grafo é conexo, menos que isso se for desconexo (floresta
// geradora mínima). *peso_total recebe a soma dos pesos. Custo: O(E log E).
int kruskal(int n, Aresta *arestas, int m, Aresta *mst, int *peso_total);

// Prim a partir de 'origem': preenche mst (capacidade mínima n) e retorna
// quantas arestas foram escolhidas. Só alcança o componente conexo de
// origem. *peso_total recebe a soma dos pesos. Custo: O((V + E) log V).
int prim(GrafoListaPonderado *g, int origem, Aresta *mst, int *peso_total);

#endif