#ifndef DAG_H
#define DAG_H
 
#include "grafo_lista.h"
 
// Insere uma aresta DIRIGIDA u -> v (diferente de inserir_aresta_lista, que
// marca os dois sentidos e serve para grafos não-dirigidos)
void inserir_aresta_dirigida(GrafoLista *g, int u, int v);
 
// Preenche grau_entrada[i] com o número de arestas que apontam PARA i.
// grau_entrada deve apontar para um array já alocado com g->n posições
void calcular_grau_entrada(GrafoLista *g, int *grau_entrada);
 
// Retorna 1 se o grafo não tem ciclo (é um DAG de fato), 0 caso contrário
int eh_dag(GrafoLista *g);
 
// Ordenação topológica pelo algoritmo de Kahn (BFS + grau de entrada).
// Preenche *tamanho com o número de vértices na ordenação e retorna um array
// alocado dinamicamente com essa ordem. Se o grafo tiver ciclo, retorna NULL
// e *tamanho = 0 (não existe ordenação topológica nesse caso).
int *ordenacao_topologica_kahn(GrafoLista *g, int *tamanho);
 
// Ordenação topológica via DFS (empilha cada vértice na saída da recursão,
// depois inverte a pilha). Mesma convenção de retorno que a versão de Kahn.
int *ordenacao_topologica_dfs(GrafoLista *g, int *tamanho);
 
#endif
 