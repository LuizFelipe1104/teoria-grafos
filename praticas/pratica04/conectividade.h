#ifndef CONECTIVIDADE_H
#define CONECTIVIDADE_H
 
#include "grafo_lista.h"
 
typedef struct {
    int u;
    int v;
} Aresta;
 
// DFS de Tarjan: preenche descoberta[], low[] e marca eh_articulacao[u] = 1
// quando u é vértice de articulação. Todos os arrays devem estar alocados
// com g->n posições antes da primeira chamada; tempo começa em 0 e pai em -1
// (pai = -1 indica que u é raiz da árvore de busca daquele componente).
// filhos_raiz é usado só quando u é raiz, para aplicar a regra especial dela.
void dfs_articulacoes(GrafoLista *g, int u, int pai, int *visitado,
                       int *descoberta, int *low, int *tempo,
                       int *eh_articulacao, int *filhos_raiz);
 
// Roda dfs_articulacoes a partir de todo vértice não visitado (cobre grafos
// desconexos) e retorna um array alocado dinamicamente com os vértices de
// articulação encontrados; *quantidade recebe o tamanho desse array.
int *encontrar_articulacoes(GrafoLista *g, int *quantidade);
 
// Mesma ideia de Tarjan, mas para pontes: uma aresta (u, v) é ponte quando
// low[v] > descoberta[u]. Retorna um array de Aresta alocado dinamicamente,
// preenchendo *quantidade com o número de pontes encontradas.
Aresta *detectar_pontes(GrafoLista *g, int *quantidade);
 
#endif