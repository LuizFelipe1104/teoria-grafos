#ifndef HAMILTONIANO_H
#define HAMILTONIANO_H

#include "grafo_lista.h"

int encontrar_caminho_hamiltoniano_bt(GrafoLista *g, int *caminho);
int encontrar_ciclo_hamiltoniano_bt(GrafoLista *g, int *ciclo);
int eh_caminho_hamiltoniano(GrafoLista *g, int *caminho);
int eh_ciclo_hamiltoniano(GrafoLista *g, int *ciclo);

#endif