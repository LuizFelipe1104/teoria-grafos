#ifndef COBERTURA_H
#define COBERTURA_H

#include "grafo_lista.h"

// Uma cobertura de vértices é um conjunto C de vértices tal que TODA aresta
// do grafo tem pelo menos uma ponta em C. Aqui ela é representada por um
// array 'cobertura' de g->n posições: cobertura[v] = 1 se v está em C, 0 se não.
// (O array deve ser alocado pelo chamador.)

// Retorna 1 se 'cobertura' cobre todas as arestas de g, 0 caso contrário.
int eh_cobertura(GrafoLista *g, int *cobertura);

// FORÇA BRUTA com backtracking e poda por limite: encontra uma cobertura de
// tamanho MÍNIMO (resposta exata, mas tempo exponencial no pior caso).
// Preenche 'cobertura' e retorna o tamanho dela.
// Se nos_visitados != NULL, *nos_visitados recebe quantas chamadas
// recursivas foram feitas (medida de esforço independente do relógio).
int cobertura_minima_bt(GrafoLista *g, int *cobertura, long *nos_visitados);

// HEURÍSTICA gulosa: enquanto sobrar aresta descoberta, coloca na cobertura
// o vértice de maior grau restante (nº de arestas ainda descobertas que
// tocam nele). Rápida, mas NÃO garante o mínimo.
// Preenche 'cobertura' e retorna o tamanho dela.
int cobertura_gulosa(GrafoLista *g, int *cobertura);

#endif