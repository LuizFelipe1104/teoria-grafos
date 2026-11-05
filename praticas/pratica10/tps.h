#ifndef TPS_H
#define TPS_H

#include "grafo_lista.h"

typedef struct {
    int n;
    int **dist;
} GrafoTSP;

GrafoTSP *criar_grafo_tsp(int n);                 // todas as distâncias = 0
void definir_distancia(GrafoTSP *t, int u, int v, int d); // simétrica: u-v e v-u
void liberar_grafo_tsp(GrafoTSP *t);
GrafoTSP *grafo_para_tsp(GrafoLista *g);


int custo_rota(GrafoTSP *t, int *rota);
int tsp_forca_bruta(GrafoTSP *t, int *rota, long *permutacoes);
int tsp_vizinho_mais_proximo(GrafoTSP *t, int *rota);
int tsp_insercao(GrafoTSP *t, int *rota);

#endif