#pragma once

#define MAPA_LINHAS 16
#define MAPA_COLUNAS 20
#define TAM_BLOCOS 48

#include "../midia.h"

enum EBloco {
    N, // Nada
    C, // Cacto
    P, // Pedra
    A, // Arbusto
};

enum EDecoracao {
    DN, // Nada
    DG, // Grama
    DP, // Pedrinhas
};

void desenhar_mapa(FolhaSprites sprites);

int colide_no_cenario(int x, int y, int tam_box);