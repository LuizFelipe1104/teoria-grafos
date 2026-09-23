#ifndef BUSCA_PROFUNDIDADE_H
#define BUSCA_PROFUNDIDADE_H
 
#include "grafo_lista.h"
 
// Pilha (LIFO) — não é usada pela dfs_recursiva (que usa a pilha de chamadas
// da própria recursão), mas fica disponível caso queira uma DFS iterativa também
typedef struct {
    int *dados;
    int topo, capacidade;
} Pilha;
 
Pilha *criar_pilha(int capacidade);
int pilha_vazia(Pilha *p);
void empilhar(Pilha *p, int valor);
int desempilhar(Pilha *p);
void liberar_pilha(Pilha *p);
 
// visitado, entrada e saida devem apontar para arrays já alocados com g->n posições;
// tempo é um contador global passado por referência (começa em 0 antes da primeira chamada)
void dfs_recursiva(GrafoLista *g, int u, int *visitado, int *tempo, int *entrada, int *saida);
 
int contar_componentes(GrafoLista *g);
int tem_ciclo(GrafoLista *g);
 
#endif