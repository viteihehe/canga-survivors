#pragma once

#include <allegro5/allegro_audio.h>

typedef struct {
    ALLEGRO_BITMAP *canga;

    ALLEGRO_BITMAP *tatu;
    ALLEGRO_BITMAP *formiga;
    ALLEGRO_BITMAP *cuspe;

    ALLEGRO_BITMAP *areia;
    ALLEGRO_BITMAP *cacto;
    ALLEGRO_BITMAP *pedra;
    ALLEGRO_BITMAP *arbusto;

    ALLEGRO_BITMAP *sombra;
    ALLEGRO_BITMAP *bala;
    ALLEGRO_BITMAP *grama;
    ALLEGRO_BITMAP *pedrinhas;

} FolhaSprites;

typedef struct {
    ALLEGRO_AUDIO_STREAM *musica_de_fundo;
    ALLEGRO_SAMPLE *morte_inimigos;
    ALLEGRO_SAMPLE *disparo;
    ALLEGRO_SAMPLE *hit;
    ALLEGRO_AUDIO_STREAM *musica_derrota;
    ALLEGRO_SAMPLE *hit_inimigo;
} Som;
