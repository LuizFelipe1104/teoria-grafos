#include <stdlib.h>
#include "dijkstra.h"

// ======================= Grafo ponderado =======================

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

// ======================= Min-Heap =======================

MinHeap *criar_min_heap(int capacidade) {
    MinHeap *h = malloc(sizeof(MinHeap));
    h->dados = malloc(capacidade * sizeof(NoHeap));
    h->posicao = malloc(capacidade * sizeof(int));
    h->capacidade = capacidade;
    h->tamanho = 0;
    for (int i = 0; i < capacidade; i++) {
        h->posicao[i] = -1; // -1 = esse vértice não está no heap
    }
    return h;
}

int heap_vazio(MinHeap *h) {
    return h->tamanho == 0;
}

int heap_contem(MinHeap *h, int vertice) {
    return h->posicao[vertice] != -1;
}

// Troca dois nós do heap E atualiza o mapa posicao[] dos dois vértices.
// Esquecer de atualizar posicao[] é o erro mais comum nessa estrutura.
static void trocar(MinHeap *h, int i, int j) {
    NoHeap tmp = h->dados[i];
    h->dados[i] = h->dados[j];
    h->dados[j] = tmp;

    h->posicao[h->dados[i].vertice] = i;
    h->posicao[h->dados[j].vertice] = j;
}

// Sobe o nó do índice i enquanto ele for menor que o pai.
static void subir(MinHeap *h, int i) {
    while (i > 0) {
        int pai = (i - 1) / 2;
        if (h->dados[pai].distancia <= h->dados[i].distancia) break;
        trocar(h, pai, i);
        i = pai;
    }
}

// Desce o nó do índice i enquanto ele for maior que algum filho.
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

    // move o último nó para a raiz e "desce" ele até a posição certa
    h->tamanho--;
    h->posicao[minimo.vertice] = -1; // saiu do heap

    if (h->tamanho > 0) {
        h->dados[0] = h->dados[h->tamanho];
        h->posicao[h->dados[0].vertice] = 0;
        descer(h, 0);
    }

    return minimo;
}

// Graças ao posicao[], achamos o vértice no heap em O(1) e só precisamos
// subir ele (a chave só diminui), custando O(log V) no total.
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

// ======================= Dijkstra =======================

void dijkstra(GrafoListaPonderado *g, int origem, int *dist, int *pred) {
    int n = g->n;

    for (int i = 0; i < n; i++) {
        dist[i] = INFINITO;
        pred[i] = -1;
    }
    dist[origem] = 0;

    // todos os vértices entram no heap; só a origem começa com distância 0
    MinHeap *heap = criar_min_heap(n);
    for (int i = 0; i < n; i++) {
        inserir_heap(heap, i, dist[i]);
    }

    while (!heap_vazio(heap)) {
        NoHeap atual = extrair_minimo(heap);
        int u = atual.vertice;

        // se o menor que sobrou ainda é infinito, o resto é inalcançável
        if (dist[u] == INFINITO) break;

        NoPonderado *aresta = g->adj[u];
        while (aresta != NULL) {
            int v = aresta->destino;
            int nova = dist[u] + aresta->peso;

            // relaxamento: achamos um caminho melhor até v passando por u?
            if (heap_contem(heap, v) && nova < dist[v]) {
                dist[v] = nova;
                pred[v] = u;
                diminuir_chave(heap, v, nova);
            }
            aresta = aresta->prox;
        }
    }

    liberar_min_heap(heap);
}

int *reconstruir_caminho(int origem, int destino, int *pred) {
    // destino sem predecessor só é válido se ele for a própria origem
    if (destino != origem && pred[destino] == -1) {
        return NULL;
    }

    // 1ª passada: conta quantos vértices há no caminho
    int tamanho = 1;
    for (int v = destino; v != origem; v = pred[v]) {
        tamanho++;
    }

    int *caminho = malloc((tamanho + 1) * sizeof(int));
    caminho[tamanho] = -1; // terminador

    // 2ª passada: preenche de trás para frente (do destino até a origem)
    int idx = tamanho - 1;
    for (int v = destino; v != origem; v = pred[v]) {
        caminho[idx--] = v;
    }
    caminho[0] = origem;

    return caminho;
}