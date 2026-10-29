#ifndef DIJKSTRA_H
#define DIJKSTRA_H

#include <limits.h>

#define INFINITO INT_MAX

// ---------- Grafo com lista de adjacência ponderada ----------

typedef struct NoPonderado {
    int destino;
    int peso;
    struct NoPonderado *prox;
} NoPonderado;

typedef struct {
    int n;               // número de vértices
    NoPonderado **adj;   // adj[u] = lista de arestas que saem de u
} GrafoListaPonderado;

GrafoListaPonderado *criar_grafo_ponderado(int n);
// Insere a aresta nos DOIS sentidos (grafo não-direcionado) com o mesmo peso.
void inserir_aresta_ponderada(GrafoListaPonderado *g, int u, int v, int peso);
void liberar_grafo_ponderado(GrafoListaPonderado *g);

// ---------- Min-Heap (fila de prioridade) ----------

typedef struct {
    int vertice;
    int distancia;
} NoHeap;

typedef struct {
    NoHeap *dados;
    int capacidade, tamanho;
    int *posicao; // mapeia vértice -> índice no heap (-1 se não está no heap)
} MinHeap;

// capacidade = número máximo de vértices (os vértices vão de 0 a capacidade-1)
MinHeap *criar_min_heap(int capacidade);
int heap_vazio(MinHeap *h);
int heap_contem(MinHeap *h, int vertice);
void inserir_heap(MinHeap *h, int vertice, int distancia);
NoHeap extrair_minimo(MinHeap *h);
void diminuir_chave(MinHeap *h, int vertice, int nova_distancia);
void liberar_min_heap(MinHeap *h);

// ---------- Dijkstra ----------

// dist e pred devem apontar para arrays já alocados com g->n posições.
// Ao final: dist[v] = menor distância de origem até v (INFINITO se inalcançável)
//           pred[v] = vértice anterior a v no caminho mínimo (-1 se não houver)
// Só funciona corretamente para pesos NÃO-NEGATIVOS.
void dijkstra(GrafoListaPonderado *g, int origem, int *dist, int *pred);

// Monta o caminho origem -> destino a partir do vetor pred.
// Retorna um array alocado dinamicamente com os vértices do caminho, na ordem
// origem ... destino, TERMINADO POR -1 (o caller deve dar free).
// Retorna NULL se destino for inalcançável a partir de origem.
int *reconstruir_caminho(int origem, int destino, int *pred);

#endif