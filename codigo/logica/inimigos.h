#pragma once

#include "jogador.h"
#include <allegro5/bitmap.h>

typedef enum { CIMA, BAIXO, DIREITA, ESQUERDA } Direcoes;

typedef enum {
    TATU,
    FORMIGA,
} EComportamento;

typedef struct {
    EComportamento comportamento;
    int tamanho_box;
    int ultimo_ataque;
    int posx;
    int posy;
    ALLEGRO_BITMAP *sprite;
    int vida;
    int dano;
    float velocidade;
    bool ativo;
    int total_frames;
    int frame_atual;
    int tamanho_sprite;
    int vida_max;
    int contador_frames;
} Inimigo;

void criarInimigo(
    Inimigo **inimigos,
    FolhaSprites sprites,
    double *ultimo_spawn_inimigo,
    int *quant_inimigos,
    double *cooldoown_inimigos,
    int comportamento,
    int *contador_total
);

void inimigosLogica(
    Inimigo inimigos[], int *indice, Jogador canga, double *counts
);

void colisaoInimigos(
    Inimigo inimigos[], int *indice, int tamanho, int tamanhosprite
);

void colisaoBala(
    Bala *bala_atual, Inimigo *inimigo_atual, int colisao, Som som
);

void processamentoBala(
    Inimigo inimigos[],
    int *indice,
    Bala balas[],
    int *max_balas,
    int colisao,
    Jogador *canga,
    Som *sons,
    int *contador_morte
);

void reajusteInimigos(Inimigo inimigos[], int *indice);

void desenharInimigo(Inimigo inimigos[], int indice, Jogador canga);

void danoJogador(
    Inimigo inimigos[], Jogador *canga, int indice, double counts, Som som
);

void desenhar_vida_inimigos(Inimigo *inimigos, int quant_inimigos);