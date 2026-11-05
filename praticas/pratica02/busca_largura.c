#include <stdlib.h>
#include "busca_largura.h"
 
Fila *criar_fila(int capacidade) {
    Fila *f = malloc(sizeof(Fila));
    f->dados = malloc(capacidade * sizeof(int));
    f->capacidade = capacidade;
    f->inicio = 0;
    f->fim = 0;
    f->tamanho = 0;
    return f;
}
 
int fila_vazia(Fila *f) {
    return f->tamanho == 0;
}
 
void enfileirar(Fila *f, int valor) {
    f->dados[f->fim] = valor;
    f->fim = (f->fim + 1) % f->capacidade; // array circular: volta ao início ao passar do fim
    f->tamanho++;
}
 
int desenfileirar(Fila *f) {
    int valor = f->dados[f->inicio];
    f->inicio = (f->inicio + 1) % f->capacidade;
    f->tamanho--;
    return valor;
}
 
void liberar_fila(Fila *f) {
    free(f->dados);
    free(f);
}
 
void bfs(GrafoLista *g, int origem, int *dist, int *pred) {
    // -1 em dist funciona como "não visitado ainda"
    for (int i = 0; i < g->n; i++) {
        dist[i] = -1;
        pred[i] = -1;
    }
 
    Fila *fila = criar_fila(g->n);
    dist[origem] = 0;
    enfileirar(fila, origem);
 
    while (!fila_vazia(fila)) {
        int u = desenfileirar(fila);
        No *atual = g->adj[u];
 
        while (atual != NULL) {
            int v = atual->destino;
            if (dist[v] == -1) { // só visita quem ainda não tem distância definida
                dist[v] = dist[u] + 1;
                pred[v] = u;
                enfileirar(fila, v);
            }
            atual = atual->prox;
        }
    }
 
    liberar_fila(fila);
}
 
int eh_bipartido(GrafoLista *g) {
    int *cor = malloc(g->n * sizeof(int)); // -1 = sem cor, 0 e 1 = as duas cores
    for (int i = 0; i < g->n; i++) cor[i] = -1;
 
    Fila *fila = criar_fila(g->n);
 
    // percorre todos os vértices para cobrir grafos desconexos
    for (int s = 0; s < g->n; s++) {
        if (cor[s] != -1) continue;
 
        cor[s] = 0;
        enfileirar(fila, s);
 
        while (!fila_vazia(fila)) {
            int u = desenfileirar(fila);
            No *atual = g->adj[u];
 
            while (atual != NULL) {
                int v = atual->destino;
                if (cor[v] == -1) {
                    cor[v] = 1 - cor[u]; // pinta com a cor oposta à de u
                    enfileirar(fila, v);
                } else if (cor[v] == cor[u]) {
                    // vizinho com a mesma cor => não dá pra 2-colorir => não é bipartido
                    liberar_fila(fila);
                    free(cor);
                    return 0;
                }
                atual = atual->prox;
            }
        }
    }
 
    liberar_fila(fila);
    free(cor);
    return 1;
}
 