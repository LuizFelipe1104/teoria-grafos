#ifndef COLORACAO_H
#define COLORACAO_H
 
#include "grafo_lista.h"
 
// Colore os vértices na ordem natural (0, 1, 2, ..., n-1) — "ordenação
// arbitrária" no sentido de que não segue nenhum critério (grau, etc.), só
// a ordem em que os vértices existem. Cada vértice recebe a menor cor que
// nenhum vizinho já colorido esteja usando.
// Retorna um array de tamanho g->n com a cor de cada vértice; *num_cores
// recebe quantas cores distintas foram usadas. Caller deve dar free().
int *coloracao_gulosa(GrafoLista *g, int *num_cores);
 
// Mesmo algoritmo guloso, mas processando os vértices em ordem de GRAU
// DECRESCENTE antes de colorir (heurística de Welsh-Powell). Isso tende a
// usar menos cores que a ordem arbitrária, mas não garante o número
// cromático ótimo (coloração ótima é NP-difícil em geral).
int *coloracao_welsh_powell(GrafoLista *g, int *num_cores);
 
// Testa se o grafo é bipartido via BFS com 2-coloração. Diferente das duas
// funções acima (que são heurísticas), esta dá uma resposta EXATA para o
// caso particular de número cromático <= 2: o grafo é bipartido se e só se
// existe uma 2-coloração válida.
int eh_bipartido(GrafoLista *g);
 
#endif