#include <stdio.h>
#include <stdlib.h>
#include "grafo_lista.h"
#include "dag.h"
 
static void imprimir_ordem(const char *rotulo, int *ordem, int tamanho) {
    if (ordem == NULL) {
        printf("%s: NULL (grafo tem ciclo, nao existe ordenacao topologica)\n", rotulo);
        return;
    }
 
    printf("%s:", rotulo);
    for (int i = 0; i < tamanho; i++) {
        printf(" %d", ordem[i]);
    }
    printf("\n");
}
 
int main() {
    // ---- Exemplo 1: um DAG de verdade ----
    // 5 -> 2, 5 -> 0, 4 -> 0, 4 -> 1, 2 -> 3, 3 -> 1
    int n1 = 6;
    GrafoLista *dag = criar_grafo_lista(n1);
    inserir_aresta_dirigida(dag, 5, 2);
    inserir_aresta_dirigida(dag, 5, 0);
    inserir_aresta_dirigida(dag, 4, 0);
    inserir_aresta_dirigida(dag, 4, 1);
    inserir_aresta_dirigida(dag, 2, 3);
    inserir_aresta_dirigida(dag, 3, 1);
 
    printf("=== Grafo 1 (DAG) ===\n");
    printf("E DAG? %s\n", eh_dag(dag) ? "Sim" : "Nao");
 
    int tamanho;
    int *ordem_kahn = ordenacao_topologica_kahn(dag, &tamanho);
    imprimir_ordem("Ordem (Kahn)", ordem_kahn, tamanho);
    free(ordem_kahn);
 
    int *ordem_dfs = ordenacao_topologica_dfs(dag, &tamanho);
    imprimir_ordem("Ordem (DFS) ", ordem_dfs, tamanho);
    free(ordem_dfs);
 
    liberar_grafo_lista(dag);
 
    // ---- Exemplo 2: o mesmo grafo, mas com uma aresta a mais que fecha um ciclo ----
    // adiciona 1 -> 4, criando o ciclo 4 -> 1 -> 4
    printf("\n=== Grafo 2 (com ciclo) ===\n");
    int n2 = 6;
    GrafoLista *com_ciclo = criar_grafo_lista(n2);
    inserir_aresta_dirigida(com_ciclo, 5, 2);
    inserir_aresta_dirigida(com_ciclo, 5, 0);
    inserir_aresta_dirigida(com_ciclo, 4, 0);
    inserir_aresta_dirigida(com_ciclo, 4, 1);
    inserir_aresta_dirigida(com_ciclo, 2, 3);
    inserir_aresta_dirigida(com_ciclo, 3, 1);
    inserir_aresta_dirigida(com_ciclo, 1, 4); // fecha o ciclo
 
    printf("E DAG? %s\n", eh_dag(com_ciclo) ? "Sim" : "Nao");
 
    ordem_kahn = ordenacao_topologica_kahn(com_ciclo, &tamanho);
    imprimir_ordem("Ordem (Kahn)", ordem_kahn, tamanho);
    free(ordem_kahn);
 
    ordem_dfs = ordenacao_topologica_dfs(com_ciclo, &tamanho);
    imprimir_ordem("Ordem (DFS) ", ordem_dfs, tamanho);
    free(ordem_dfs);
 
    liberar_grafo_lista(com_ciclo);
 
    return 0;
}
 