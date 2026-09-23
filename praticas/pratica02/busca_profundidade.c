#include <stdlib.h>
#include "busca_profundidade.h"
 
Pilha *criar_pilha(int capacidade) {
    Pilha *p = malloc(sizeof(Pilha));
    p->dados = malloc(capacidade * sizeof(int));
    p->capacidade = capacidade;
    p->topo = -1; // pilha vazia começa com topo = -1
    return p;
}
 
int pilha_vazia(Pilha *p) {
    return p->topo == -1;
}
 
void empilhar(Pilha *p, int valor) {
    p->topo++;
    p->dados[p->topo] = valor;
}
 
int desempilhar(Pilha *p) {
    int valor = p->dados[p->topo];
    p->topo--;
    return valor;
}
 
void liberar_pilha(Pilha *p) {
    free(p->dados);
    free(p);
}
 
void dfs_recursiva(GrafoLista *g, int u, int *visitado, int *tempo, int *entrada, int *saida) {
    visitado[u] = 1;
    entrada[u] = (*tempo)++; // marca quando "entrou" no vértice e avança o relógio
 
    No *atual = g->adj[u];
    while (atual != NULL) {
        int v = atual->destino;
        if (!visitado[v]) {
            dfs_recursiva(g, v, visitado, tempo, entrada, saida);
        }
        atual = atual->prox;
    }
 
    saida[u] = (*tempo)++; // marca quando "saiu" do vértice, já tendo visitado tudo alcançável a partir dele
}
 
int contar_componentes(GrafoLista *g) {
    int *visitado = calloc(g->n, sizeof(int));
    int *entrada = malloc(g->n * sizeof(int));
    int *saida = malloc(g->n * sizeof(int));
    int tempo = 0;
    int componentes = 0;
 
    for (int i = 0; i < g->n; i++) {
        if (!visitado[i]) {
            dfs_recursiva(g, i, visitado, &tempo, entrada, saida);
            componentes++; // cada DFS que precisa começar de novo é um componente novo
        }
    }
 
    free(visitado);
    free(entrada);
    free(saida);
    return componentes;
}
 
// DFS auxiliar para detectar ciclo usando 3 estados (como cores):
// 0 = branco (não visitado), 1 = cinza (no caminho atual da recursão), 2 = preto (finalizado)
// 'pai' evita confundir a aresta de volta ao pai (normal em grafo não-direcionado) com um ciclo de verdade
static int dfs_ciclo(GrafoLista *g, int u, int *estado, int pai) {
    estado[u] = 1;
 
    No *atual = g->adj[u];
    while (atual != NULL) {
        int v = atual->destino;
        if (estado[v] == 0) {
            if (dfs_ciclo(g, v, estado, u)) return 1;
        } else if (estado[v] == 1 && v != pai) {
            // achou um vértice cinza (ainda no caminho atual) que não é o pai -> existe ciclo
            return 1;
        }
        atual = atual->prox;
    }
 
    estado[u] = 2;
    return 0;
}
 
int tem_ciclo(GrafoLista *g) {
    int *estado = calloc(g->n, sizeof(int));
 
    for (int i = 0; i < g->n; i++) {
        if (estado[i] == 0) {
            if (dfs_ciclo(g, i, estado, -1)) {
                free(estado);
                return 1;
            }
        }
    }
 
    free(estado);
    return 0;
}