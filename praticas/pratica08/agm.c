#include <stdlib.h>
#include <string.h>
#include "agm.h"

// ======================= Grafo ponderado não-direcionado =======================

GrafoListaPonderado *criar_grafo_ponderado(int n) {
    GrafoListaPonderado *g = malloc(sizeof(GrafoListaPonderado));
    g->n = n;
    g->adj = malloc(n * sizeof(NoPonderado *));
    for (int i = 0; i < n; i++) {
        g->adj[i] = NULL;
    }
    return g;
}

static void inserir_no_ponderado(NoPonderado **cabeca, int destino, int peso) {
    NoPonderado *novo = malloc(sizeof(NoPonderado));
    novo->destino = destino;
    novo->peso = peso;
    novo->prox = *cabeca;
    *cabeca = novo;
}

void inserir_aresta_ponderada(GrafoListaPonderado *g, int u, int v, int peso) {
    inserir_no_ponderado(&g->adj[u], v, peso);
    inserir_no_ponderado(&g->adj[v], u, peso);
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

Aresta *extrair_arestas(GrafoListaPonderado *g, int *m) {
    // Cada aresta u-v aparece duas vezes (em adj[u] e em adj[v]); pegamos só
    // a ocorrência em que u < v para contá-la uma única vez.
    int total = 0;
    for (int u = 0; u < g->n; u++) {
        for (NoPonderado *a = g->adj[u]; a != NULL; a = a->prox) {
            if (u < a->destino) total++;
        }
    }

    Aresta *arestas = malloc((total > 0 ? total : 1) * sizeof(Aresta));
    int idx = 0;
    for (int u = 0; u < g->n; u++) {
        for (NoPonderado *a = g->adj[u]; a != NULL; a = a->prox) {
            if (u < a->destino) {
                arestas[idx].u = u;
                arestas[idx].v = a->destino;
                arestas[idx].peso = a->peso;
                idx++;
            }
        }
    }

    *m = total;
    return arestas;
}

// ======================= Union-Find =======================

DSU *criar_dsu(int n) {
    DSU *d = malloc(sizeof(DSU));
    d->n = n;
    d->pai = malloc(n * sizeof(int));
    d->rank = calloc(n, sizeof(int));
    for (int i = 0; i < n; i++) {
        d->pai[i] = i; // cada elemento começa sozinho, sendo a própria raiz
    }
    return d;
}

void liberar_dsu(DSU *d) {
    free(d->pai);
    free(d->rank);
    free(d);
}

int find(DSU *d, int x) {
    if (d->pai[x] != x) {
        // compressão de caminho: todo nó visitado passa a apontar direto
        // para a raiz, deixando as próximas buscas quase instantâneas
        d->pai[x] = find(d, d->pai[x]);
    }
    return d->pai[x];
}

int unir(DSU *d, int x, int y) {
    int rx = find(d, x);
    int ry = find(d, y);

    if (rx == ry) return 0; // mesmo conjunto: unir fecharia um ciclo

    // união por rank: a árvore mais "baixa" é pendurada na mais "alta",
    // o que mantém as árvores rasas
    if (d->rank[rx] < d->rank[ry]) {
        d->pai[rx] = ry;
    } else if (d->rank[rx] > d->rank[ry]) {
        d->pai[ry] = rx;
    } else {
        d->pai[ry] = rx;
        d->rank[rx]++; // só cresce quando duas árvores de mesma altura se juntam
    }
    return 1;
}

// ======================= Min-Heap =======================

MinHeap *criar_min_heap(int capacidade) {
    MinHeap *h = malloc(sizeof(MinHeap));
    h->dados = malloc(capacidade * sizeof(NoHeap));
    h->posicao = malloc(capacidade * sizeof(int));
    h->capacidade = capacidade;
    h->tamanho = 0;
    for (int i = 0; i < capacidade; i++) {
        h->posicao[i] = -1;
    }
    return h;
}

int heap_vazio(MinHeap *h) {
    return h->tamanho == 0;
}

int heap_contem(MinHeap *h, int vertice) {
    return h->posicao[vertice] != -1;
}

static void trocar(MinHeap *h, int i, int j) {
    NoHeap tmp = h->dados[i];
    h->dados[i] = h->dados[j];
    h->dados[j] = tmp;

    h->posicao[h->dados[i].vertice] = i;
    h->posicao[h->dados[j].vertice] = j;
}

static void subir(MinHeap *h, int i) {
    while (i > 0) {
        int pai = (i - 1) / 2;
        if (h->dados[pai].distancia <= h->dados[i].distancia) break;
        trocar(h, pai, i);
        i = pai;
    }
}

static void descer(MinHeap *h, int i) {
    while (1) {
        int esq = 2 * i + 1;
        int dir = 2 * i + 2;
        int menor = i;

        if (esq < h->tamanho && h->dados[esq].distancia < h->dados[menor].distancia) {
            menor = esq;
        }
        if (dir < h->tamanho && h->dados[dir].distancia < h->dados[menor].distancia) {
            menor = dir;
        }
        if (menor == i) break;

        trocar(h, i, menor);
        i = menor;
    }
}

void inserir_heap(MinHeap *h, int vertice, int distancia) {
    int i = h->tamanho;
    h->dados[i].vertice = vertice;
    h->dados[i].distancia = distancia;
    h->posicao[vertice] = i;
    h->tamanho++;
    subir(h, i);
}

NoHeap extrair_minimo(MinHeap *h) {
    NoHeap minimo = h->dados[0];

    h->tamanho--;
    h->posicao[minimo.vertice] = -1;

    if (h->tamanho > 0) {
        h->dados[0] = h->dados[h->tamanho];
        h->posicao[h->dados[0].vertice] = 0;
        descer(h, 0);
    }

    return minimo;
}

void diminuir_chave(MinHeap *h, int vertice, int nova_distancia) {
    int i = h->posicao[vertice];
    h->dados[i].distancia = nova_distancia;
    subir(h, i);
}

void liberar_min_heap(MinHeap *h) {
    free(h->dados);
    free(h->posicao);
    free(h);
}

// ======================= Kruskal =======================

static int comparar_arestas(const void *a, const void *b) {
    const Aresta *x = a;
    const Aresta *y = b;
    if (x->peso < y->peso) return -1;
    if (x->peso > y->peso) return 1;
    return 0;
}

int kruskal(int n, Aresta *arestas, int m, Aresta *mst, int *peso_total) {
    // ordena uma CÓPIA, para não bagunçar o vetor de quem chamou
    Aresta *ordenadas = malloc((m > 0 ? m : 1) * sizeof(Aresta));
    memcpy(ordenadas, arestas, m * sizeof(Aresta));
    qsort(ordenadas, m, sizeof(Aresta), comparar_arestas); // O(E log E)

    DSU *d = criar_dsu(n);
    int escolhidas = 0;
    *peso_total = 0;

    // guloso: pega a aresta mais barata que ainda NÃO fecha ciclo
    for (int i = 0; i < m && escolhidas < n - 1; i++) {
        if (unir(d, ordenadas[i].u, ordenadas[i].v)) {
            mst[escolhidas++] = ordenadas[i];
            *peso_total += ordenadas[i].peso;
        }
    }

    liberar_dsu(d);
    free(ordenadas);
    return escolhidas;
}

// ======================= Prim =======================

int prim(GrafoListaPonderado *g, int origem, Aresta *mst, int *peso_total) {
    int n = g->n;

    int *chave = malloc(n * sizeof(int)); // menor peso de aresta ligando v à árvore
    int *pai = malloc(n * sizeof(int));   // vértice da árvore do outro lado dessa aresta
    for (int i = 0; i < n; i++) {
        chave[i] = INFINITO;
        pai[i] = -1;
    }
    chave[origem] = 0;

    MinHeap *heap = criar_min_heap(n);
    for (int i = 0; i < n; i++) {
        inserir_heap(heap, i, chave[i]);
    }

    int escolhidas = 0;
    *peso_total = 0;

    while (!heap_vazio(heap)) {
        NoHeap atual = extrair_minimo(heap);
        int u = atual.vertice;

        // se o menor que sobrou é infinito, o resto está em outro componente
        if (chave[u] == INFINITO) break;

        // u acaba de entrar na árvore (saiu do heap); registra a aresta que o trouxe
        if (pai[u] != -1) {
            mst[escolhidas].u = pai[u];
            mst[escolhidas].v = u;
            mst[escolhidas].peso = chave[u];
            escolhidas++;
            *peso_total += chave[u];
        }

        // atualiza as chaves dos vizinhos que ainda estão fora da árvore
        for (NoPonderado *a = g->adj[u]; a != NULL; a = a->prox) {
            int v = a->destino;
            if (heap_contem(heap, v) && a->peso < chave[v]) {
                chave[v] = a->peso;
                pai[v] = u;
                diminuir_chave(heap, v, a->peso);
            }
        }
    }

    liberar_min_heap(heap);
    free(chave);
    free(pai);
    return escolhidas;
}