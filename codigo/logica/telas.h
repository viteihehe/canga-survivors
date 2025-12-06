#pragma once

#include "../constantes.h"
#include "../midia.h"
#include "../utils.h"
#include "quicksort.h"
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/bitmap.h>
#include <allegro5/bitmap_draw.h>
#include <allegro5/bitmap_io.h>
#include <stdio.h>

typedef struct {
    int nivel_atingido;
    int tempo_de_jogo;
    int Dano_causado;
    int Dano_sofrido;
    int passos_dados;
    int total_inimigos_mortos;
    int pontuacao_run;
} Estatisticas;

void desenhar_pontuacao(int pontos, ALLEGRO_FONT *fonte);

void selecao_letra(
    ALLEGRO_EVENT evento,
    char *letra,
    bool *selecionou,
    ALLEGRO_SAMPLE *selecao,
    ALLEGRO_SAMPLE *escolha
);

void exibir_escrita_sigla(
    ALLEGRO_FONT *fonte,
    int *op,
    bool *selecionou,
    char sigla[],
    char *letra,
    int x,
    int y,
    char texto[]
);

void tela_morte(
    ALLEGRO_EVENT evento,
    int pontos,
    ALLEGRO_FONT *fonte_menor,
    ALLEGRO_FONT *fonte,
    ALLEGRO_FONT *fonte_frase,
    char sigla[],
    char *letra,
    int *op,
    bool *selecionou,
    ALLEGRO_SAMPLE *escolha,
    ALLEGRO_SAMPLE *selecao,
    ALLEGRO_BITMAP *caveira,
    Estatisticas est
);

void salvar_arquivo(int pontos, char sigla[]);

void exibir_lista(
    ALLEGRO_FONT *fonte,
    ALLEGRO_FONT *fonte_titulo,
    int x1,
    int y1,
    int x2,
    int y2,
    int tipo
);

void busca_pontucao(
    ALLEGRO_EVENT evento,
    ALLEGRO_FONT *fonte,
    ALLEGRO_FONT *fonte_titulo,
    int *op,
    bool *selecionou,
    char *letra,
    ALLEGRO_SAMPLE *escolha,
    ALLEGRO_SAMPLE *selecao,
    char sigla_busca[]
);

Usuarios *retorna_score(char sigla[]);

void desenhar_guias(FolhaSprites sprites, ALLEGRO_FONT *fonte);
