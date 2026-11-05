#include <stdlib.h>
#include "hamiltoniano.h"

// Estado compartilhado pela recursão
typedef struct {
    GrafoLista *g;
    int *caminho;     // caminho[0..pos-1] já está montado
    int *visitado;    // visitado[v] = 1 se v já está no caminho
    int n;
    int exige_ciclo;  // 1 = o último vértice também precisa ligar ao primeiro
} Contexto;

// Tenta completar o caminho a partir da posição 'pos'. Retorna 1 se conseguiu.
static int estender(Contexto *c, int pos) {
    if (pos == c->n) {
        // todos os vértices usados: caminho completo; para ciclo, falta
        // conferir a aresta de volta ao início
        if (!c->exige_ciclo) return 1;
        return sao_adjacentes_lista(c->g, c->caminho[pos - 1], c->caminho[0]);
    }

    int ultimo = c->caminho[pos - 1];

    // só tentamos os vizinhos do último vértice (e não todos os n vértices):
    // é isso que poda as permutações que usariam arestas inexistentes
    for (No *a = c->g->adj[ultimo]; a != NULL; a = a->prox) {
        int v = a->destino;
        if (c->visitado[v]) continue;

        c->visitado[v] = 1;
        c->caminho[pos] = v;
        if (estender(c, pos + 1)) return 1;
        c->visitado[v] = 0; // desfaz a escolha (backtracking)
    }

    return 0;
}

// Roda a busca começando de cada vértice em 'inicios' (de 0 a ate-1)
static int buscar(GrafoLista *g, int *saida, int exige_ciclo, int ate) {
    int n = g->n;
    Contexto c;
    c.g = g;
    c.n = n;
    c.exige_ciclo = exige_ciclo;
    c.caminho = saida;
    c.visitado = calloc(n, sizeof(int));

    int achou = 0;
    for (int inicio = 0; inicio < ate && !achou; inicio++) {
        c.visitado[inicio] = 1;
        c.caminho[0] = inicio;
        achou = estender(&c, 1);
        if (!achou) c.visitado[inicio] = 0;
    }

    free(c.visitado);
    return achou;
}

int encontrar_caminho_hamiltoniano_bt(GrafoLista *g, int *caminho) {
    if (g->n < 1) return 0;
    // um caminho pode começar em qualquer vértice: tenta todos como início
    return buscar(g, caminho, 0, g->n);
}

int encontrar_ciclo_hamiltoniano_bt(GrafoLista *g, int *ciclo) {
    if (g->n < 3) return 0;
    // um ciclo hamiltoniano passa por todos os vértices, inclusive o 0, então
    // podemos fixar o início em 0 sem perder nenhum ciclo
    return buscar(g, ciclo, 1, 1);
}

// ======================= Validadores =======================

static int eh_permutacao(int n, int *v) {
    int *visto = calloc(n, sizeof(int));
    int ok = 1;
    for (int i = 0; i < n && ok; i++) {
        if (v[i] < 0 || v[i] >= n || visto[v[i]]) ok = 0;
        else visto[v[i]] = 1;
    }
    free(visto);
    return ok;
}

int eh_caminho_hamiltoniano(GrafoLista *g, int *caminho) {
    int n = g->n;
    if (n < 1 || !eh_permutacao(n, caminho)) return 0;
    for (int i = 0; i + 1 < n; i++) {
        if (!sao_adjacentes_lista(g, caminho[i], caminho[i + 1])) return 0;
    }
    return 1;
}

int eh_ciclo_hamiltoniano(GrafoLista *g, int *ciclo) {
    if (g->n < 3) return 0;
    if (!eh_caminho_hamiltoniano(g, ciclo)) return 0;
    return sao_adjacentes_lista(g, ciclo[g->n - 1], ciclo[0]);
}