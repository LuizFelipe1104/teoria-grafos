#include "planaridade.h"
 
int eh_planar_euler(GrafoLista *g) {
    int n = g->n;
    if (n < 3) return 1; // a fórmula 3n-6 só é definida (e relevante) a partir de n=3
 
    int soma_graus = 0;
    for (int i = 0; i < n; i++) {
        soma_graus += grau_lista(g, i);
    }
    int m = soma_graus / 2; // cada aresta é contada duas vezes (uma por extremidade)
 
    return m <= 3 * n - 6;
}
 
static int contar_bits(int mascara) {
    int c = 0;
    while (mascara) {
        c += mascara & 1;
        mascara >>= 1;
    }
    return c;
}
 
int contem_k5_subgrafo(GrafoLista *g) {
    int n = g->n;
    if (n < 5) return 0;
 
    // percorre todo subconjunto de vértices representável em n bits (viável
    // só porque essa função é chamada apenas para n pequeno, ex. <= 10)
    for (int mascara = 0; mascara < (1 << n); mascara++) {
        if (contar_bits(mascara) != 5) continue;
 
        int verts[5], idx = 0;
        for (int i = 0; i < n; i++) {
            if (mascara & (1 << i)) verts[idx++] = i;
        }
 
        int completo = 1;
        for (int i = 0; i < 5 && completo; i++) {
            for (int j = i + 1; j < 5 && completo; j++) {
                if (!sao_adjacentes_lista(g, verts[i], verts[j])) {
                    completo = 0;
                }
            }
        }
 
        if (completo) return 1; // achou 5 vértices todos ligados entre si
    }
 
    return 0;
}
 
int contem_k33_subgrafo(GrafoLista *g) {
    int n = g->n;
    if (n < 6) return 0;
 
    for (int mascara = 0; mascara < (1 << n); mascara++) {
        if (contar_bits(mascara) != 6) continue;
 
        int verts[6], idx = 0;
        for (int i = 0; i < n; i++) {
            if (mascara & (1 << i)) verts[idx++] = i;
        }
 
        // agora tenta toda forma de separar esses 6 vértices em dois grupos de 3
        // (usa uma "sub-máscara" de 6 bits, de 0 a 63, independente da máscara acima)
        for (int sub = 0; sub < 64; sub++) {
            if (contar_bits(sub) != 3) continue;
 
            int a[3], b[3], ia = 0, ib = 0;
            for (int i = 0; i < 6; i++) {
                if (sub & (1 << i)) {
                    a[ia++] = verts[i];
                } else {
                    b[ib++] = verts[i];
                }
            }
 
            int completo = 1;
            for (int i = 0; i < 3 && completo; i++) {
                for (int j = 0; j < 3 && completo; j++) {
                    if (!sao_adjacentes_lista(g, a[i], b[j])) {
                        completo = 0;
                    }
                }
            }
 
            if (completo) return 1; // achou uma bipartição 3x3 totalmente conectada
        }
    }
 
    return 0;
}
 
int eh_planar_heuristica(GrafoLista *g) {
    if (!eh_planar_euler(g)) {
        return 0; // já falhou na condição necessária, nem precisa checar K5/K3,3
    }
 
    if (g->n <= 10) {
        if (contem_k5_subgrafo(g)) return 0;
        if (contem_k33_subgrafo(g)) return 0;
    }
    // para n > 10 a força bruta fica inviável; nesse caso a função só se apoia
    // na condição de Euler, que é mais fraca (ver aviso no header)
 
    return 1;
}