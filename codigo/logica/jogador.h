#pragma once

#include "../midia.h"
#include <allegro5/bitmap.h>
#include <allegro5/events.h>

#define VEL_BALA 10 // Em frames

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

    MapaDirecoes movimento;
    MapaDirecoes mira;

    int cooldown_arma;
    long tempo_ultimo_disparo;

    int vida;
    int dano_delay;
    int dano;
    float velocidade;
    bool vivo;
    int ultimo_dano;

    Som sons;
    int xp;
} Jogador;

typedef struct {
    ALLEGRO_BITMAP *sprite;
    int x;
    int y;
    MapaDirecoes direcoes;
    bool ativa;
    int dano;
} Bala;

void capturar_movimento(ALLEGRO_EVENT evento, MapaDirecoes *teclas);

void capturar_mira(ALLEGRO_EVENT evento, MapaDirecoes *teclas);

void mover_jogador(MapaDirecoes teclas, Jogador *jogador);

void criar_bala_jogador(Bala **balas, int *dest_quant, Jogador *jogador,
                        ALLEGRO_TIMER *tick_timer, FolhaSprites sprites,
                        Som som);

void mover_balas(Bala *balas, int quant_balas);
