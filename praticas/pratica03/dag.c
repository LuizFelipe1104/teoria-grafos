#include <stdlib.h>
#include "dag.h"
 
void inserir_aresta_dirigida(GrafoLista *g, int u, int v) {
    // igual à inserção usada no grafo_lista, mas só no sentido u -> v
    No *novo = malloc(sizeof(No));
    novo->destino = v;
    novo->prox = g->adj[u];
    g->adj[u] = novo;
}
 
void calcular_grau_entrada(GrafoLista *g, int *grau_entrada) {
    for (int i = 0; i < g->n; i++) {
        grau_entrada[i] = 0;
    }
 
    // para cada aresta u -> v que existe no grafo, v ganha +1 no grau de entrada
    for (int u = 0; u < g->n; u++) {
        No *atual = g->adj[u];
        while (atual != NULL) {
            grau_entrada[atual->destino]++;
            atual = atual->prox;
        }
    }
}
 
int *ordenacao_topologica_kahn(GrafoLista *g, int *tamanho) {
    int n = g->n;
    int *grau_entrada = malloc(n * sizeof(int));
    calcular_grau_entrada(g, grau_entrada);
 
    // fila simples em array: como cada vértice entra e sai da fila no máximo
    // uma vez, um array de tamanho n é suficiente (não precisa ser circular)
    int *fila = malloc(n * sizeof(int));
    int inicio = 0, fim = 0;
 
    // todo vértice com grau de entrada 0 pode ser o "primeiro" na ordenação
    for (int i = 0; i < n; i++) {
        if (grau_entrada[i] == 0) {
            fila[fim++] = i;
        }
    }
 
    int *ordem = malloc(n * sizeof(int));
    int count = 0;
 
    while (inicio < fim) {
        int u = fila[inicio++];
        ordem[count++] = u;
 
        // "remove" u do grafo: diminui o grau de entrada de quem dependia dele
        No *atual = g->adj[u];
        while (atual != NULL) {
            int v = atual->destino;
            grau_entrada[v]--;
            if (grau_entrada[v] == 0) {
                fila[fim++] = v; // v ficou livre de dependências, entra na fila
            }
            atual = atual->prox;
        }
    }
 
    free(grau_entrada);
    free(fila);
 
    if (count != n) {
        // sobrou vértice com grau de entrada > 0 que nunca zerou -> tem ciclo,
        // então não existe ordenação topológica válida
        free(ordem);
        *tamanho = 0;
        return NULL;
    }
 
    *tamanho = count;
    return ordem;
}
 
static void dfs_topologica(GrafoLista *g, int u, int *visitado, int *pilha, int *topo) {
    visitado[u] = 1;
 
    No *atual = g->adj[u];
    while (atual != NULL) {
        int v = atual->destino;
        if (!visitado[v]) {
            dfs_topologica(g, v, visitado, pilha, topo);
        }
        atual = atual->prox;
    }
 
    // só empilha u depois de esgotar TUDO que é alcançável a partir dele
    pilha[(*topo)++] = u;
}
 
int *ordenacao_topologica_dfs(GrafoLista *g, int *tamanho) {
    // uma DFS "ingênua" não percebe ciclo sozinha (ela ainda produziria uma
    // ordem, só que inválida), então checamos antes com eh_dag
    if (!eh_dag(g)) {
        *tamanho = 0;
        return NULL;
    }
 
    int n = g->n;
    int *visitado = calloc(n, sizeof(int));
    int *pilha = malloc(n * sizeof(int));
    int topo = 0;
 
    for (int i = 0; i < n; i++) {
        if (!visitado[i]) {
            dfs_topologica(g, i, visitado, pilha, &topo);
        }
    }
    free(visitado);
 
    // quem "termina" primeiro (sai da recursão primeiro) fica no início da
    // pilha; a ordenação topológica correta é essa pilha só que invertida
    int *ordem = malloc(n * sizeof(int));
    for (int i = 0; i < n; i++) {
        ordem[i] = pilha[n - 1 - i];
    }
    free(pilha);
 
    *tamanho = n;
    return ordem;
}
 
// Detecção de ciclo em digrafo com 3 estados por vértice:
// 0 = branco (não visitado), 1 = cinza (no caminho atual da recursão), 2 = preto (finalizado)
static int dfs_ciclo_dirigido(GrafoLista *g, int u, int *estado) {
    estado[u] = 1;
 
    No *atual = g->adj[u];
    while (atual != NULL) {
        int v = atual->destino;
        if (estado[v] == 1) {
            return 1; // aresta apontando para alguém ainda "em andamento" -> ciclo
        }
        if (estado[v] == 0 && dfs_ciclo_dirigido(g, v, estado)) {
            return 1;
        }
        atual = atual->prox;
    }
 
    estado[u] = 2;
    return 0;
}
 
int eh_dag(GrafoLista *g) {
    int *estado = calloc(g->n, sizeof(int));
 
    for (int i = 0; i < g->n; i++) {
        if (estado[i] == 0) {
            if (dfs_ciclo_dirigido(g, i, estado)) {
                free(estado);
                return 0;
            }
        }
    }
 
    free(estado);
    return 1;
}