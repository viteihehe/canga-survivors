#include "score.h"
#include <stdlib.h>
#include <time.h>

int particao(int e, int d, Usuarios u[]) {
    srand(time(NULL));
    int idx = e+(rand() % (d-e));
    Usuarios temp = u[idx];
    u[idx] = u[d];
    u[d] = temp;

    int i;
    int j = e-1;
    int pivo = u[d].pontos;

    for(i = e; i < d; i++) {
        if(u[i].pontos >= pivo) {
            j++;
            temp = u[i];
            u[i] = u[j];
            u[j] = temp;
        }
    }

    temp = u[j+1];
    u[j+1] = u[d];
    u[d] = temp;
    return j+1;

}
void quicksort(int i, int f, Usuarios u[]) {
    if(i < f) {
        int p = particao(i, f, u);
        quicksort(p+1, f, u);
        quicksort(i, p-1, u);
    }
}