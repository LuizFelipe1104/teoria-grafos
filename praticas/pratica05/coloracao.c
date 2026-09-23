#include <stdlib.h>
#include "coloracao.h"
 
// Núcleo comum: dado um vetor 'ordem' com a sequência em que os vértices
// devem ser processados, aplica o algoritmo guloso de coloração.
static int *colorir_na_ordem(GrafoLista *g, int *ordem) {
    int n = g->n;
    int *cor = malloc(n * sizeof(int));
    for (int i = 0; i < n; i++) cor[i] = -1;
 
    // no pior caso um grafo com n vértices precisa de até n cores (K_n),
    // então um array de tamanho n é suficiente para marcar disponibilidade
    int *disponivel = malloc(n * sizeof(int));
 
    for (int idx = 0; idx < n; idx++) {
        int u = ordem[idx];
 
        for (int c = 0; c < n; c++) disponivel[c] = 1;
 
        // qualquer cor já usada por um vizinho já colorido fica indisponível
        No *atual = g->adj[u];
        while (atual != NULL) {
            int v = atual->destino;
            if (cor[v] != -1) {
                disponivel[cor[v]] = 0;
            }
            atual = atual->prox;
        }
 
        int c;
        for (c = 0; c < n; c++) {
            if (disponivel[c]) break;
        }
        cor[u] = c;
    }
 
    free(disponivel);
    return cor;
}
 
static int contar_cores_usadas(int *cor, int n) {
    int maior = -1;
    for (int i = 0; i < n; i++) {
        if (cor[i] > maior) maior = cor[i];
    }
    return maior + 1; // cores vão de 0 até 'maior', por isso +1
}
 
int *coloracao_gulosa(GrafoLista *g, int *num_cores) {
    int n = g->n;
    int *ordem = malloc(n * sizeof(int));
    for (int i = 0; i < n; i++) ordem[i] = i; // ordem natural, sem critério
 
    int *cor = colorir_na_ordem(g, ordem);
    free(ordem);
 
    *num_cores = contar_cores_usadas(cor, n);
    return cor;
}
 
// Insertion sort simples por grau decrescente. Estável o suficiente para o
// tamanho típico dessas práticas (não é o gargalo do algoritmo).
static void ordenar_por_grau_decrescente(GrafoLista *g, int *ordem) {
    int n = g->n;
    for (int i = 1; i < n; i++) {
        int chave = ordem[i];
        int grau_chave = grau_lista(g, chave);
        int j = i - 1;
        while (j >= 0 && grau_lista(g, ordem[j]) < grau_chave) {
            ordem[j + 1] = ordem[j];
            j--;
        }
        ordem[j + 1] = chave;
    }
}
 
int *coloracao_welsh_powell(GrafoLista *g, int *num_cores) {
    int n = g->n;
    int *ordem = malloc(n * sizeof(int));
    for (int i = 0; i < n; i++) ordem[i] = i;
 
    ordenar_por_grau_decrescente(g, ordem); // diferença única em relação ao guloso simples
 
    int *cor = colorir_na_ordem(g, ordem);
    free(ordem);
 
    *num_cores = contar_cores_usadas(cor, n);
    return cor;
}
 
int eh_bipartido(GrafoLista *g) {
    int n = g->n;
    int *cor = malloc(n * sizeof(int));
    for (int i = 0; i < n; i++) cor[i] = -1;
 
    int *fila = malloc(n * sizeof(int));
 
    for (int s = 0; s < n; s++) {
        if (cor[s] != -1) continue;
 
        int inicio = 0, fim = 0;
        cor[s] = 0;
        fila[fim++] = s;
 
        while (inicio < fim) {
            int u = fila[inicio++];
            No *atual = g->adj[u];
            while (atual != NULL) {
                int v = atual->destino;
                if (cor[v] == -1) {
                    cor[v] = 1 - cor[u];
                    fila[fim++] = v;
                } else if (cor[v] == cor[u]) {
                    free(cor);
                    free(fila);
                    return 0;
                }
                atual = atual->prox;
            }
        }
    }
 
    free(cor);
    free(fila);
    return 1;
}
 