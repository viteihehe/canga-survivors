#include <stdlib.h>
#include <allegro5/bitmap.h>

typedef struct {
    bool cima;
    bool baixo;
    bool esq;
    bool dir;
} MapaDirecoes;

typedef struct {
    ALLEGRO_BITMAP *sprite;
    int x;
    int y;
    MapaDirecoes direcoes;
    bool ativa;
    int dano;
} Bala;

typedef struct No {
    Bala dado;
    struct No * prox;
} No;

typedef struct {
    No * inicio;
} Lista;

void inserir_bala(Lista *lista, Bala bala);
void remover_balas_mortas(Lista *lista);
void liberar_lista(Lista *lista);
