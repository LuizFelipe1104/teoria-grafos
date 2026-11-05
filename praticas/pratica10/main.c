#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "grafo_lista.h"
#include "hamiltoniano.h"
#include "tps.h"

// Gerador pseudoaleatório próprio (em vez de rand()) para que o experimento
// gere EXATAMENTE as mesmas instâncias em qualquer computador/sistema.
static unsigned int semente = 2024u;

static int proximo_aleatorio(void) {
    semente = semente * 1664525u + 1013904223u;
    return (int)((semente >> 16) % 100u); // 0..99
}

// n cidades em pontos aleatórios de uma grade 100x100; a distância é a
// "distância de quarteirão" (|dx| + |dy|), que respeita a desigualdade
// triangular como numa rede de ruas de verdade.
static GrafoTSP *gerar_instancia_tsp(int n) {
    int *x = malloc(n * sizeof(int));
    int *y = malloc(n * sizeof(int));
    for (int i = 0; i < n; i++) {
        x[i] = proximo_aleatorio();
        y[i] = proximo_aleatorio();
    }

    GrafoTSP *t = criar_grafo_tsp(n);
    for (int u = 0; u < n; u++) {
        for (int v = u + 1; v < n; v++) {
            int dx = x[u] > x[v] ? x[u] - x[v] : x[v] - x[u];
            int dy = y[u] > y[v] ? y[u] - y[v] : y[v] - y[u];
            definir_distancia(t, u, v, dx + dy);
        }
    }

    free(x);
    free(y);
    return t;
}

static void imprimir_vetor(const char *rotulo, int *v, int n) {
    printf("%s", rotulo);
    for (int i = 0; i < n; i++) {
        printf("%s%d", i == 0 ? "" : " -> ", v[i]);
    }
    printf("\n");
}

// ---- Grafos de teste da parte 1 e 2 ----

// Hexagono 0-1-2-3-4-5-0 com uma corda 0-3: tem ciclo (e portanto caminho)
static GrafoLista *grafo_hexagono_com_corda(void) {
    GrafoLista *g = criar_grafo_lista(6);
    for (int i = 0; i < 6; i++) inserir_aresta_lista(g, i, (i + 1) % 6);
    inserir_aresta_lista(g, 0, 3);
    return g;
}

// Grafo de Petersen: famoso por TER caminho hamiltoniano mas NAO ter ciclo
static GrafoLista *grafo_petersen(void) {
    GrafoLista *g = criar_grafo_lista(10);
    for (int i = 0; i < 5; i++) {
        inserir_aresta_lista(g, i, (i + 1) % 5);          // pentagono externo
        inserir_aresta_lista(g, i, i + 5);                // raios
        inserir_aresta_lista(g, 5 + i, 5 + (i + 2) % 5);  // estrela interna
    }
    return g;
}

// Estrela: o centro 0 ligado a 1..4. Nao tem caminho nem ciclo hamiltoniano,
// porque ir de uma folha a outra sempre exige passar pelo centro de novo.
static GrafoLista *grafo_estrela(void) {
    GrafoLista *g = criar_grafo_lista(5);
    for (int i = 1; i < 5; i++) inserir_aresta_lista(g, 0, i);
    return g;
}

static void testar_hamiltoniano(const char *nome, GrafoLista *g) {
    int n = g->n;
    int *caminho = malloc(n * sizeof(int));
    int *ciclo = malloc(n * sizeof(int));

    printf("--- %s (%d vertices) ---\n", nome, n);

    if (encontrar_caminho_hamiltoniano_bt(g, caminho)) {
        imprimir_vetor("Caminho hamiltoniano: ", caminho, n);
        printf("  validador confirma? %s\n", eh_caminho_hamiltoniano(g, caminho) ? "sim" : "NAO");
    } else {
        printf("Caminho hamiltoniano: nao existe\n");
    }

    if (encontrar_ciclo_hamiltoniano_bt(g, ciclo)) {
        imprimir_vetor("Ciclo hamiltoniano:   ", ciclo, n);
        printf("  (e volta para %d)  validador confirma? %s\n", ciclo[0],
               eh_ciclo_hamiltoniano(g, ciclo) ? "sim" : "NAO");
    } else {
        printf("Ciclo hamiltoniano:   nao existe\n");
    }

    free(caminho);
    free(ciclo);
}

// Reducao Ciclo Hamiltoniano -> TSP: o ciclo existe sse o TSP otimo custa n
static void testar_reducao(const char *nome, GrafoLista *g) {
    int n = g->n;
    GrafoTSP *t = grafo_para_tsp(g);
    int *rota = malloc(n * sizeof(int));
    int *ciclo = malloc(n * sizeof(int));

    long perm;
    int otimo = tsp_forca_bruta(t, rota, &perm);
    int tem_ciclo = encontrar_ciclo_hamiltoniano_bt(g, ciclo);

    printf("%-26s n=%2d | TSP otimo = %2d | %s | backtracking diz: %s\n",
           nome, n, otimo,
           otimo == n ? "custo == n (ciclo)    " : "custo >  n (sem ciclo)",
           tem_ciclo ? "tem ciclo" : "sem ciclo");

    free(rota);
    free(ciclo);
    liberar_grafo_tsp(t);
}

// ---- Medicao de tempo ----

static int forca_bruta_sem_contador(GrafoTSP *t, int *rota) {
    return tsp_forca_bruta(t, rota, NULL);
}

// Tempo medio (em microssegundos) de uma execucao. Repete ate acumular ~50 ms,
// porque uma unica execucao das heuristicas e rapida demais para o relogio.
static double medir(int (*algoritmo)(GrafoTSP *, int *), GrafoTSP *t, int *rota) {
    clock_t inicio = clock();
    long repeticoes = 0;
    do {
        algoritmo(t, rota);
        repeticoes++;
    } while (clock() - inicio < CLOCKS_PER_SEC / 20);
    double segundos = (double)(clock() - inicio) / CLOCKS_PER_SEC;
    return segundos / repeticoes * 1e6;
}

static double gap_percentual(int custo, int otimo) {
    return otimo == 0 ? 0.0 : 100.0 * (custo - otimo) / otimo;
}

int main() {
    // ================= Parte 1: caminho e ciclo hamiltoniano =================
    printf("=== Parte 1: caminho e ciclo hamiltoniano (backtracking) ===\n\n");
    GrafoLista *hex = grafo_hexagono_com_corda();
    GrafoLista *pet = grafo_petersen();
    GrafoLista *est = grafo_estrela();

    testar_hamiltoniano("Hexagono com corda", hex);
    printf("\n");
    testar_hamiltoniano("Grafo de Petersen (caminho SIM, ciclo NAO)", pet);
    printf("\n");
    testar_hamiltoniano("Estrela (nenhum dos dois)", est);

    // ================= Parte 2: reducao para TSP =================
    printf("\n=== Parte 2: reducao Ciclo Hamiltoniano -> TSP (peso 1 = aresta, 2 = nao-aresta) ===\n");
    testar_reducao("Hexagono com corda", hex);
    testar_reducao("Petersen", pet);
    testar_reducao("Estrela", est);

    liberar_grafo_lista(hex);
    liberar_grafo_lista(pet);
    liberar_grafo_lista(est);

    // ================= Parte 3: experimento n = 5, 8, 10, 12 =================
    printf("\n=== Parte 3: experimento (1 instancia aleatoria por n; tempos em microssegundos) ===\n");
    printf("gap = quanto o custo da heuristica passa do otimo, em %%.\n\n");
    printf("%3s | %7s %12s %11s | %7s %6s %9s | %7s %6s %9s\n",
           "n", "otimo", "tempo FB", "(n-1)!", "viz.", "gap", "tempo", "inser.", "gap", "tempo");
    printf("----------------------------------------------------------------------------------------\n");

    int tamanhos[] = {5, 8, 10, 12};
    int quantidade = sizeof(tamanhos) / sizeof(tamanhos[0]);

    for (int k = 0; k < quantidade; k++) {
        int n = tamanhos[k];
        GrafoTSP *t = gerar_instancia_tsp(n);
        int *rota = malloc(n * sizeof(int));

        long perm;
        int otimo = tsp_forca_bruta(t, rota, &perm);
        int c_viz = tsp_vizinho_mais_proximo(t, rota);
        int c_ins = tsp_insercao(t, rota);

        double t_fb = medir(forca_bruta_sem_contador, t, rota);
        double t_viz = medir(tsp_vizinho_mais_proximo, t, rota);
        double t_ins = medir(tsp_insercao, t, rota);

        printf("%3d | %7d %12.1f %11ld | %7d %5.1f%% %9.2f | %7d %5.1f%% %9.2f\n",
               n, otimo, t_fb, perm,
               c_viz, gap_percentual(c_viz, otimo), t_viz,
               c_ins, gap_percentual(c_ins, otimo), t_ins);

        free(rota);
        liberar_grafo_tsp(t);
    }

    // ================= Parte 4: qualidade media das heuristicas =================
    // Uma unica instancia diz pouco sobre qualidade; aqui sao 30 instancias
    // de n = 9 (8! = 40320 rotas cada, barato para a forca bruta).
    printf("\n=== Parte 4: qualidade media das heuristicas (30 instancias, n = 9) ===\n");
    int n = 9, instancias = 30;
    double soma_gap_viz = 0, soma_gap_ins = 0, pior_viz = 0, pior_ins = 0;
    int otimos_viz = 0, otimos_ins = 0, ins_melhor = 0, viz_melhor = 0;
    int *rota = malloc(n * sizeof(int));

    for (int i = 0; i < instancias; i++) {
        GrafoTSP *t = gerar_instancia_tsp(n);
        int otimo = tsp_forca_bruta(t, rota, NULL);
        int c_viz = tsp_vizinho_mais_proximo(t, rota);
        int c_ins = tsp_insercao(t, rota);

        double g_viz = gap_percentual(c_viz, otimo);
        double g_ins = gap_percentual(c_ins, otimo);
        soma_gap_viz += g_viz;
        soma_gap_ins += g_ins;
        if (g_viz > pior_viz) pior_viz = g_viz;
        if (g_ins > pior_ins) pior_ins = g_ins;
        if (c_viz == otimo) otimos_viz++;
        if (c_ins == otimo) otimos_ins++;
        if (c_ins < c_viz) ins_melhor++;
        if (c_viz < c_ins) viz_melhor++;

        liberar_grafo_tsp(t);
    }
    free(rota);

    printf("%-22s | gap medio | pior gap | achou o otimo\n", "Heuristica");
    printf("%-22s | %8.1f%% | %7.1f%% | %d de %d\n", "Vizinho mais proximo",
           soma_gap_viz / instancias, pior_viz, otimos_viz, instancias);
    printf("%-22s | %8.1f%% | %7.1f%% | %d de %d\n", "Insercao do mais prox.",
           soma_gap_ins / instancias, pior_ins, otimos_ins, instancias);
    printf("Insercao venceu o vizinho em %d instancia(s); vizinho venceu em %d; empataram em %d.\n",
           ins_melhor, viz_melhor, instancias - ins_melhor - viz_melhor);

    return 0;
}