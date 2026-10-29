#include <stdio.h>
#include <stdlib.h>
#include "bellman_ford.h"

// ======================= Grafo ponderado direcionado =======================

GrafoListaPonderado *criar_grafo_ponderado(int n) {
    GrafoListaPonderado *g = malloc(sizeof(GrafoListaPonderado));
    g->n = n;
    g->adj = malloc(n * sizeof(NoPonderado *));
    for (int i = 0; i < n; i++) {
        g->adj[i] = NULL;
    }
    return g;
}

void inserir_aresta_ponderada(GrafoListaPonderado *g, int u, int v, int peso) {
    NoPonderado *novo = malloc(sizeof(NoPonderado));
    novo->destino = v;
    novo->peso = peso;
    novo->prox = g->adj[u];
    g->adj[u] = novo;
}

void liberar_grafo_ponderado(GrafoListaPonderado *g) {
    for (int i = 0; i < g->n; i++) {
        NoPonderado *atual = g->adj[i];
        while (atual != NULL) {
            NoPonderado *tmp = atual;
            atual = atual->prox;
            free(tmp);
        }
    }
    free(g->adj);
    free(g);
}

// ======================= Bellman-Ford =======================

int bellman_ford(GrafoListaPonderado *g, int origem, int *dist, int *pred) {
    int n = g->n;

    for (int i = 0; i < n; i++) {
        dist[i] = INFINITO;
        pred[i] = -1;
    }
    dist[origem] = 0;

    // Um caminho mínimo simples tem no máximo n-1 arestas. Cada passada pelo
    // conjunto de TODAS as arestas garante a distância correta para os
    // caminhos com uma aresta a mais; por isso bastam n-1 passadas.
    for (int iteracao = 1; iteracao <= n - 1; iteracao++) {
        int mudou = 0;

        for (int u = 0; u < n; u++) {
            if (dist[u] == INFINITO) continue; // u ainda não foi alcançado

            NoPonderado *aresta = g->adj[u];
            while (aresta != NULL) {
                int v = aresta->destino;
                int nova = dist[u] + aresta->peso;

                // relaxamento: passar por u melhora o caminho até v?
                if (nova < dist[v]) {
                    dist[v] = nova;
                    pred[v] = u;
                    mudou = 1;
                }
                aresta = aresta->prox;
            }
        }

        // se uma passada inteira não melhorou nada, as próximas também não
        if (!mudou) return 1;
    }

    // n-ésima passada (só verificação, sem alterar nada): se ainda existe
    // aresta que melhora alguma distância, é porque há um ciclo negativo
    // alcançável — dá para "andar nele" para sempre diminuindo o custo.
    for (int u = 0; u < n; u++) {
        if (dist[u] == INFINITO) continue;

        NoPonderado *aresta = g->adj[u];
        while (aresta != NULL) {
            if (dist[u] + aresta->peso < dist[aresta->destino]) {
                return 0;
            }
            aresta = aresta->prox;
        }
    }

    return 1;
}

// Menor peso entre as arestas u -> v (pode haver arestas paralelas)
static int peso_aresta(GrafoListaPonderado *g, int u, int v) {
    int melhor = INFINITO;
    NoPonderado *atual = g->adj[u];
    while (atual != NULL) {
        if (atual->destino == v && atual->peso < melhor) {
            melhor = atual->peso;
        }
        atual = atual->prox;
    }
    return melhor;
}

int reconstruir_ciclo_negativo(GrafoListaPonderado *g, int *dist, int *pred) {
    int n = g->n;

    // trabalha numa cópia de pred para não alterar o array do chamador
    int *p = malloc(n * sizeof(int));
    for (int i = 0; i < n; i++) p[i] = pred[i];

    // 1) acha uma aresta u -> v que ainda poderia ser relaxada (é a "n-ésima
    //    relaxação" do algoritmo); registra u como predecessor de v
    int x = -1;
    for (int u = 0; u < n && x == -1; u++) {
        if (dist[u] == INFINITO) continue;

        NoPonderado *aresta = g->adj[u];
        while (aresta != NULL) {
            int v = aresta->destino;
            if (dist[u] + aresta->peso < dist[v]) {
                p[v] = u;
                x = v;
                break;
            }
            aresta = aresta->prox;
        }
    }

    if (x == -1) { // nenhum ciclo negativo
        free(p);
        return 0;
    }

    // 2) x pode estar "pendurado" num rabo que leva ao ciclo, e não dentro
    //    dele. Retroceder n vezes pelo pred[] garante cair dentro do ciclo,
    //    porque qualquer rabo tem no máximo n-1 vértices.
    for (int i = 0; i < n; i++) {
        x = p[x];
    }

    // 3) agora x está no ciclo: percorre pred até voltar em x, coletando
    int *ciclo = malloc((n + 1) * sizeof(int));
    int tamanho = 0;
    int atual = x;
    do {
        ciclo[tamanho++] = atual;
        atual = p[atual];
    } while (atual != x);

    // andando por pred vamos "para trás" (v, depois quem chegou em v, ...);
    // inverte para imprimir o ciclo no sentido real das arestas
    for (int i = 0; i < tamanho / 2; i++) {
        int tmp = ciclo[i];
        ciclo[i] = ciclo[tamanho - 1 - i];
        ciclo[tamanho - 1 - i] = tmp;
    }

    // 4) imprime e soma o custo do ciclo
    long custo = 0;
    printf("Ciclo negativo: ");
    for (int i = 0; i < tamanho; i++) {
        int de = ciclo[i];
        int para = ciclo[(i + 1) % tamanho];
        custo += peso_aresta(g, de, para);
        printf("%d -> ", de);
    }
    printf("%d  (custo total = %ld)\n", ciclo[0], custo);

    free(ciclo);
    free(p);
    return tamanho;
}