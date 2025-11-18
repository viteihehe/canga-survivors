#pragma once

#include "../midia.h"
#include <allegro5/bitmap.h>
#include <allegro5/events.h>
#include "lista.h"

#define VEL_BALA 10 // Em frames

typedef struct {
    ALLEGRO_BITMAP *sprite;

    int x;
    int y;

    MapaDirecoes movimento;
    MapaDirecoes mira;

    int cooldown_arma;
    long tempo_ultimo_disparo;

    bool powerup_pronto;

    int vida;
    int dano_delay;
    int dano;
    float velocidade;
    bool vivo;
    int ultimo_dano;
    int pontuacao;
    Lista balas;

    Som sons;
} Jogador;

void capturar_movimento(ALLEGRO_EVENT evento, MapaDirecoes *teclas);

void capturar_mira(ALLEGRO_EVENT evento, MapaDirecoes *teclas);

void mover_jogador(MapaDirecoes teclas, Jogador *jogador);

void criar_bala_jogador(
    Jogador *jogador,
    ALLEGRO_TIMER *tick_timer,
    FolhaSprites sprites,
    Som som
);

void mover_balas(Lista *lista);

void desenhar_vida_jogador(Jogador *canga, FolhaSprites sprites);