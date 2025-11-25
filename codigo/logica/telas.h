#pragma once

#include "../utils.h"
#include "../constantes.h"
#include <stdio.h>
#include <allegro5/allegro_primitives.h>
#include "quicksort.h"

void desenhar_pontuacao(int pontos, ALLEGRO_FONT* fonte);

void tela_morte(
    ALLEGRO_EVENT evento,
    int pontos,
    ALLEGRO_FONT *fonte_titulo,
    ALLEGRO_FONT *fonte,
    char sigla[],
    char *letra,
    int *op,
    bool *selecionou,
    ALLEGRO_SAMPLE *escolha,
    ALLEGRO_SAMPLE *selecao
); 

#include "../constantes.h"
#include "../utils.h"
#include "quicksort.h"
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_audio.h>
#include <stdio.h>

void desenhar_pontuacao(int pontos, ALLEGRO_FONT *fonte);

void selecao_letra(ALLEGRO_EVENT evento, char *letra, bool *selecionou, ALLEGRO_SAMPLE *selecao, ALLEGRO_SAMPLE *escolha);
              
void exibir_escrita_sigla(ALLEGRO_FONT *fonte, int *op, bool *selecionou, char sigla[], char *letra);

void tela_morte(
    ALLEGRO_EVENT evento,
    int pontos,
    ALLEGRO_FONT *fonte_titulo,
    ALLEGRO_FONT *fonte,
    char sigla[],
    char *letra,
    int *op,
    bool *selecionou,
    ALLEGRO_SAMPLE *escolha,
    ALLEGRO_SAMPLE *selecao
);


void salvar_arquivo(int pontos, char sigla[]);

void exibir_lista(ALLEGRO_FONT* fonte, ALLEGRO_FONT* fonte_titulo);

void busca_pontucao(ALLEGRO_EVENT evento, ALLEGRO_FONT* fonte, ALLEGRO_FONT* fonte_titulo, int *op, bool *selecionou, char *letra, ALLEGRO_SAMPLE *escolha,
    ALLEGRO_SAMPLE *selecao, char sigla_busca[]);

Usuarios* retorna_score(char sigla []);
