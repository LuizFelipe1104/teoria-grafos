#include <stdlib.h>
#include "conectividade.h"
 
void dfs_articulacoes(GrafoLista *g, int u, int pai, int *visitado,
                       int *descoberta, int *low, int *tempo,
                       int *eh_articulacao, int *filhos_raiz) {
    visitado[u] = 1;
    descoberta[u] = low[u] = (*tempo)++;
    int filhos = 0;
 
    No *atual = g->adj[u];
    while (atual != NULL) {
        int v = atual->destino;
 
        if (v == pai) {
            // ignora a aresta de volta direta para quem te trouxe até aqui
            atual = atual->prox;
            continue;
        }
 
        if (visitado[v]) {
            // aresta de retorno (v já está "no caminho"): low[u] pode melhorar
            // usando o tempo de descoberta de v
            if (descoberta[v] < low[u]) {
                low[u] = descoberta[v];
            }
        } else {
            filhos++;
            dfs_articulacoes(g, v, u, visitado, descoberta, low, tempo, eh_articulacao, filhos_raiz);
 
            if (low[v] < low[u]) {
                low[u] = low[v];
            }
 
            // regra de Tarjan: se u não é raiz e o "low" do filho v não
            // consegue "escapar" para antes de u, então u é ponto de corte
            if (pai != -1 && low[v] >= descoberta[u]) {
                eh_articulacao[u] = 1;
            }
        }
 
        atual = atual->prox;
    }
 
    if (pai == -1) {
        *filhos_raiz = filhos;
        // caso especial da raiz: só é articulação se tiver 2+ filhos na busca
        if (filhos > 1) {
            eh_articulacao[u] = 1;
        }
    }
}
 
int *encontrar_articulacoes(GrafoLista *g, int *quantidade) {
    int n = g->n;
    int *visitado = calloc(n, sizeof(int));
    int *descoberta = malloc(n * sizeof(int));
    int *low = malloc(n * sizeof(int));
    int *eh_articulacao = calloc(n, sizeof(int));
    int tempo = 0;
 
    for (int i = 0; i < n; i++) {
        if (!visitado[i]) {
            int filhos_raiz = 0;
            dfs_articulacoes(g, i, -1, visitado, descoberta, low, &tempo, eh_articulacao, &filhos_raiz);
        }
    }
 
    int count = 0;
    for (int i = 0; i < n; i++) {
        if (eh_articulacao[i]) count++;
    }
 
    int *resultado = malloc((count > 0 ? count : 1) * sizeof(int));
    int idx = 0;
    for (int i = 0; i < n; i++) {
        if (eh_articulacao[i]) resultado[idx++] = i;
    }
 
    free(visitado);
    free(descoberta);
    free(low);
    free(eh_articulacao);
 
    *quantidade = count;
    return resultado;
}
 
static void dfs_pontes(GrafoLista *g, int u, int pai, int *visitado,
                        int *descoberta, int *low, int *tempo,
                        Aresta *pontes, int *quantidade) {
    visitado[u] = 1;
    descoberta[u] = low[u] = (*tempo)++;
 
    No *atual = g->adj[u];
    while (atual != NULL) {
        int v = atual->destino;
 
        if (v == pai) {
            atual = atual->prox;
            continue;
        }
 
        if (visitado[v]) {
            if (descoberta[v] < low[u]) {
                low[u] = descoberta[v];
            }
        } else {
            dfs_pontes(g, v, u, visitado, descoberta, low, tempo, pontes, quantidade);
 
            if (low[v] < low[u]) {
                low[u] = low[v];
            }
 
            // condição de ponte: diferente da articulação, aqui a comparação
            // é ESTRITA (>). Isso significa que a única forma de v alcançar
            // "para trás" é justamente pela aresta u-v, então removê-la
            // desconecta o grafo
            if (low[v] > descoberta[u]) {
                pontes[*quantidade].u = u;
                pontes[*quantidade].v = v;
                (*quantidade)++;
            }
        }
 
        atual = atual->prox;
    }
}
 
Aresta *detectar_pontes(GrafoLista *g, int *quantidade) {
    int n = g->n;
    int *visitado = calloc(n, sizeof(int));
    int *descoberta = malloc(n * sizeof(int));
    int *low = malloc(n * sizeof(int));
 
    // no pior caso (uma árvore), toda aresta é ponte, e uma árvore com n
    // vértices tem no máximo n-1 arestas -- n já é um limite superior seguro
    Aresta *pontes = malloc((n > 0 ? n : 1) * sizeof(Aresta));
    int tempo = 0;
    *quantidade = 0;
 
    for (int i = 0; i < n; i++) {
        if (!visitado[i]) {
            dfs_pontes(g, i, -1, visitado, descoberta, low, &tempo, pontes, quantidade);
        }
    }
 
    free(visitado);
    free(descoberta);
    free(low);
 
    return pontes;
}
 