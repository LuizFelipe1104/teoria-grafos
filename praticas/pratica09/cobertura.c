#include <stdlib.h>
#include <string.h>
#include "cobertura.h"

int eh_cobertura(GrafoLista *g, int *cobertura) {
    for (int u = 0; u < g->n; u++) {
        for (No *a = g->adj[u]; a != NULL; a = a->prox) {
            // aresta u-v descoberta: nenhuma das duas pontas está na cobertura
            if (!cobertura[u] && !cobertura[a->destino]) {
                return 0;
            }
        }
    }
    return 1;
}

// ======================= Backtracking =======================

// Estado compartilhado pela recursão (evita passar 6 parâmetros a cada chamada)
typedef struct {
    GrafoLista *g;
    int *atual;       // cobertura parcial que está sendo construída
    int *melhor;      // melhor cobertura completa encontrada até agora
    int tam_melhor;   // tamanho de 'melhor'
    int *usado;       // área de trabalho para o limite inferior
    long nos;         // quantas chamadas recursivas foram feitas
} Contexto;

// Procura a primeira aresta u-v que ainda não está coberta.
// Retorna 1 e preenche *pu, *pv se achou; retorna 0 se todas estão cobertas.
static int primeira_aresta_descoberta(Contexto *c, int *pu, int *pv) {
    for (int u = 0; u < c->g->n; u++) {
        if (c->atual[u]) continue;
        for (No *a = c->g->adj[u]; a != NULL; a = a->prox) {
            if (!c->atual[a->destino]) {
                *pu = u;
                *pv = a->destino;
                return 1;
            }
        }
    }
    return 0;
}

// LIMITE INFERIOR: quantos vértices AINDA precisaremos, no mínimo, para
// cobrir as arestas descobertas. Monta um emparelhamento guloso (conjunto de
// arestas descobertas sem ponta em comum). Como as arestas do emparelhamento
// não compartilham vértices, cada uma exige um vértice DIFERENTE na cobertura,
// logo precisamos de pelo menos tantos vértices quanto arestas no emparelhamento.
static int limite_inferior(Contexto *c) {
    int n = c->g->n;
    memset(c->usado, 0, n * sizeof(int));
    int lb = 0;

    for (int u = 0; u < n; u++) {
        if (c->atual[u] || c->usado[u]) continue;
        for (No *a = c->g->adj[u]; a != NULL; a = a->prox) {
            int v = a->destino;
            if (!c->atual[v] && !c->usado[v]) {
                c->usado[u] = 1;
                c->usado[v] = 1;
                lb++;
                break;
            }
        }
    }
    return lb;
}

static void busca(Contexto *c, int tam) {
    c->nos++;

    int u, v;
    if (!primeira_aresta_descoberta(c, &u, &v)) {
        // todas as arestas cobertas: é uma cobertura completa
        if (tam < c->tam_melhor) {
            c->tam_melhor = tam;
            memcpy(c->melhor, c->atual, c->g->n * sizeof(int));
        }
        return;
    }

    // PODA: mesmo no melhor cenário, esse ramo não consegue superar o melhor
    // já encontrado -> não vale a pena explorá-lo
    if (tam + limite_inferior(c) >= c->tam_melhor) {
        return;
    }

    // A aresta u-v precisa ser coberta por u OU por v: são os dois ramos
    c->atual[u] = 1;
    busca(c, tam + 1);
    c->atual[u] = 0; // desfaz a escolha (é isso que dá nome ao "backtracking")

    c->atual[v] = 1;
    busca(c, tam + 1);
    c->atual[v] = 0;
}

int cobertura_minima_bt(GrafoLista *g, int *cobertura, long *nos_visitados) {
    int n = g->n;

    Contexto c;
    c.g = g;
    c.atual = calloc(n > 0 ? n : 1, sizeof(int));
    c.melhor = malloc((n > 0 ? n : 1) * sizeof(int));
    c.usado = malloc((n > 0 ? n : 1) * sizeof(int));
    c.nos = 0;

    // ponto de partida: "todos os vértices" é sempre uma cobertura válida
    // (de tamanho n); qualquer cobertura menor que achamos a substitui
    for (int i = 0; i < n; i++) c.melhor[i] = 1;
    c.tam_melhor = n;

    busca(&c, 0);

    memcpy(cobertura, c.melhor, n * sizeof(int));
    int resultado = c.tam_melhor;
    if (nos_visitados != NULL) *nos_visitados = c.nos;

    free(c.atual);
    free(c.melhor);
    free(c.usado);
    return resultado;
}

// ======================= Heurística gulosa =======================

int cobertura_gulosa(GrafoLista *g, int *cobertura) {
    int n = g->n;
    for (int i = 0; i < n; i++) cobertura[i] = 0;

    int tam = 0;
    while (1) {
        int melhor = -1, melhor_grau = 0;

        // grau restante de u = arestas de u cuja outra ponta ainda está fora
        // da cobertura (as demais já foram cobertas)
        for (int u = 0; u < n; u++) {
            if (cobertura[u]) continue;
            int grau = 0;
            for (No *a = g->adj[u]; a != NULL; a = a->prox) {
                if (!cobertura[a->destino]) grau++;
            }
            if (grau > melhor_grau) {
                melhor_grau = grau;
                melhor = u;
            }
        }

        if (melhor == -1) break; // nenhum vértice tem aresta descoberta: acabou

        cobertura[melhor] = 1;
        tam++;
    }

    return tam;
}