#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "tps.h"

// ======================= Instância TSP =======================

GrafoTSP *criar_grafo_tsp(int n) {
    GrafoTSP *t = malloc(sizeof(GrafoTSP));
    t->n = n;
    t->dist = malloc((n > 0 ? n : 1) * sizeof(int *));
    for (int i = 0; i < n; i++) {
        t->dist[i] = calloc(n, sizeof(int));
    }
    return t;
}

void definir_distancia(GrafoTSP *t, int u, int v, int d) {
    t->dist[u][v] = d;
    t->dist[v][u] = d;
}

void liberar_grafo_tsp(GrafoTSP *t) {
    for (int i = 0; i < t->n; i++) {
        free(t->dist[i]);
    }
    free(t->dist);
    free(t);
}

GrafoTSP *grafo_para_tsp(GrafoLista *g) {
    int n = g->n;
    GrafoTSP *t = criar_grafo_tsp(n);

    // primeiro, todo par de cidades diferentes começa com peso 2 ("não é aresta")
    for (int u = 0; u < n; u++) {
        for (int v = u + 1; v < n; v++) {
            definir_distancia(t, u, v, 2);
        }
    }
    // depois, as arestas que existem de verdade baixam para peso 1
    for (int u = 0; u < n; u++) {
        for (No *a = g->adj[u]; a != NULL; a = a->prox) {
            if (a->destino != u) {
                definir_distancia(t, u, a->destino, 1);
            }
        }
    }
    return t;
}

int custo_rota(GrafoTSP *t, int *rota) {
    int n = t->n;
    int custo = 0;
    for (int i = 0; i < n; i++) {
        // (i + 1) % n faz a última cidade voltar para a primeira
        custo += t->dist[rota[i]][rota[(i + 1) % n]];
    }
    return custo;
}

// ======================= Força bruta =======================

typedef struct {
    GrafoTSP *t;
    int *rota;          // permutação sendo construída (rota[0] fixo = 0)
    int *melhor_rota;
    int melhor_custo;
    long permutacoes;   // quantas rotas completas foram avaliadas
} ContextoBF;

static void trocar(int *a, int *b) {
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

// Gera todas as permutações de rota[k..n-1] trocando cada candidato para a
// posição k. 'custo_parcial' é o custo do trecho rota[0..k-1], acumulado ao
// longo da recursão para não recalcular a rota inteira em cada folha.
static void permutar(ContextoBF *c, int k, int custo_parcial) {
    int n = c->t->n;

    if (k == n) {
        // rota completa: soma a volta da última cidade para a origem
        int custo = custo_parcial + c->t->dist[c->rota[n - 1]][c->rota[0]];
        c->permutacoes++;
        if (custo < c->melhor_custo) {
            c->melhor_custo = custo;
            memcpy(c->melhor_rota, c->rota, n * sizeof(int));
        }
        return;
    }

    for (int i = k; i < n; i++) {
        trocar(&c->rota[k], &c->rota[i]);
        permutar(c, k + 1, custo_parcial + c->t->dist[c->rota[k - 1]][c->rota[k]]);
        trocar(&c->rota[k], &c->rota[i]); // desfaz a troca
    }
}

int tsp_forca_bruta(GrafoTSP *t, int *rota, long *permutacoes) {
    int n = t->n;
    if (n < 1) {
        if (permutacoes != NULL) *permutacoes = 0;
        return 0;
    }

    ContextoBF c;
    c.t = t;
    c.rota = malloc(n * sizeof(int));
    c.melhor_rota = malloc(n * sizeof(int));
    c.melhor_custo = INT_MAX;
    c.permutacoes = 0;
    for (int i = 0; i < n; i++) c.rota[i] = i; // a cidade 0 fica fixa na posição 0

    permutar(&c, 1, 0); // só as posições 1..n-1 são permutadas: (n-1)! rotas

    memcpy(rota, c.melhor_rota, n * sizeof(int));
    int resultado = c.melhor_custo;
    if (permutacoes != NULL) *permutacoes = c.permutacoes;

    free(c.rota);
    free(c.melhor_rota);
    return resultado;
}

// ======================= Vizinho mais próximo =======================

int tsp_vizinho_mais_proximo(GrafoTSP *t, int *rota) {
    int n = t->n;
    if (n < 1) return 0;

    int *visitada = calloc(n, sizeof(int));
    rota[0] = 0;
    visitada[0] = 1;

    for (int pos = 1; pos < n; pos++) {
        int atual = rota[pos - 1];
        int melhor = -1;

        // entre as cidades ainda não visitadas, pega a mais próxima da atual
        // (em caso de empate, fica a de menor índice)
        for (int v = 0; v < n; v++) {
            if (!visitada[v] && (melhor == -1 || t->dist[atual][v] < t->dist[atual][melhor])) {
                melhor = v;
            }
        }
        rota[pos] = melhor;
        visitada[melhor] = 1;
    }

    free(visitada);
    return custo_rota(t, rota);
}

// ======================= Inserção do mais próximo =======================

int tsp_insercao(GrafoTSP *t, int *rota) {
    int n = t->n;
    if (n < 1) return 0;

    int *na_rota = calloc(n, sizeof(int));
    rota[0] = 0;
    na_rota[0] = 1;
    int tamanho = 1;

    if (n >= 2) {
        // rota inicial: a cidade 0 e a cidade mais próxima dela
        int j = -1;
        for (int v = 1; v < n; v++) {
            if (j == -1 || t->dist[0][v] < t->dist[0][j]) j = v;
        }
        rota[1] = j;
        na_rota[j] = 1;
        tamanho = 2;
    }

    while (tamanho < n) {
        // 1) escolhe a cidade k, fora da rota, mais próxima de alguma cidade da rota
        int k = -1, dist_k = INT_MAX;
        for (int v = 0; v < n; v++) {
            if (na_rota[v]) continue;
            for (int i = 0; i < tamanho; i++) {
                if (t->dist[v][rota[i]] < dist_k) {
                    dist_k = t->dist[v][rota[i]];
                    k = v;
                }
            }
        }

        // 2) acha a posição que menos aumenta o custo: inserir k entre a
        //    cidade rota[p] e a seguinte troca a aresta (a, b) por (a, k) + (k, b)
        int melhor_pos = 0, menor_aumento = INT_MAX;
        for (int p = 0; p < tamanho; p++) {
            int a = rota[p];
            int b = rota[(p + 1) % tamanho];
            int aumento = t->dist[a][k] + t->dist[k][b] - t->dist[a][b];
            if (aumento < menor_aumento) {
                menor_aumento = aumento;
                melhor_pos = p;
            }
        }

        // 3) insere k logo depois de rota[melhor_pos], empurrando o resto
        for (int i = tamanho; i > melhor_pos + 1; i--) {
            rota[i] = rota[i - 1];
        }
        rota[melhor_pos + 1] = k;
        na_rota[k] = 1;
        tamanho++;
    }

    free(na_rota);
    return custo_rota(t, rota);
}