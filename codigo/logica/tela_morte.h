#pragma once

#include "../utils.h"
#include "../constantes.h"
#include <stdio.h>
#include <allegro5/allegro_primitives.h>
#include "quicksort.h"

void desenhar_pontuacao(int pontos, ALLEGRO_FONT* fonte);

void tela_morte(int pontos, ALLEGRO_FONT* fonte_titulo, ALLEGRO_FONT* fonte,char sigla[], char* letra, int* op, bool* selecionou);

void salvar_arquivo(int pontos, char sigla[]);
void exibir_lista(ALLEGRO_FONT* fonte, ALLEGRO_FONT* fonte_titulo);
