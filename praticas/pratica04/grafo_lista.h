#ifndef GRAFO_LISTA_H
#define GRAFO_LISTA_H
 
typedef struct No {
    int destino;
    struct No *prox;
} No;
 
typedef struct {
    int n;      // número de vértices
    No **adj;   // array de n listas encadeadas (uma por vértice)
} GrafoLista;
 
GrafoLista *criar_grafo_lista(int n);
void inserir_aresta_lista(GrafoLista *grafo, int u, int v);
void remover_aresta_lista(GrafoLista *grafo, int u, int v);
int grau_lista(GrafoLista *grafo, int v);
int sao_adjacentes_lista(GrafoLista *grafo, int u, int v);
void exibir_lista(GrafoLista *grafo);
void liberar_grafo_lista(GrafoLista *grafo);
 
#endif