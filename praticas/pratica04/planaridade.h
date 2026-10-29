#ifndef PLANARIDADE_H
#define PLANARIDADE_H
 
#include "grafo_lista.h"
 
// Condição NECESSÁRIA de Euler para planaridade: m <= 3n - 6 (válida para n >= 3).
// Todo grafo planar satisfaz essa desigualdade, mas o inverso não é verdadeiro:
// existem grafos não-planares que também satisfazem m <= 3n - 6 (ex.: K3,3).
// Por isso essa função sozinha NUNCA prova que um grafo é planar, só descarta
// rapidamente os que com certeza não são.
int eh_planar_euler(GrafoLista *g);
 
// Força bruta (viável só para poucos vértices, ex.: n <= 10): testa todo
// subconjunto de 5 vértices e verifica se, entre eles, todas as 10 arestas de
// um K5 estão presentes.
int contem_k5_subgrafo(GrafoLista *g);
 
// Força bruta (viável só para poucos vértices, ex.: n <= 10): testa todo
// subconjunto de 6 vértices e toda forma de dividi-los em dois grupos de 3,
// verificando se todas as 9 arestas de um K3,3 estão presentes entre os grupos.
int contem_k33_subgrafo(GrafoLista *g);
 
// Heurística combinando Euler + busca de K5/K3,3 como SUBGRAFO direto.
// IMPORTANTE (leia antes de usar em produção/prova formal): o teorema de
// Kuratowski de verdade exige detectar SUBDIVISÕES de K5/K3,3 (com caminhos
// no lugar de arestas), não apenas esses subgrafos "diretos". Essa função é
// uma aproximação didática: pode classificar como "planar" um grafo que na
// verdade não é, caso o K5/K3,3 só apareça subdividido. Só é viável para
// grafos pequenos (n <= 10) por causa do custo combinatório.
int eh_planar_heuristica(GrafoLista *g);
 
#endif