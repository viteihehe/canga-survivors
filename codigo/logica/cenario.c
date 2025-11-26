#include "cenario.h"

int mapa_blocos[MAPA_LINHAS][MAPA_COLUNAS] = {
    {A, A, A, A, A, A, A, A, N, N, N, A, A, A, A, A, P, P, P, P},
    {A, N, N, N, P, P, N, N, N, N, N, N, N, N, N, N, P, P, P, P},
    {A, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, P, P},
    {A, N, C, N, N, N, N, P, N, N, N, N, N, N, N, N, N, N, N, P},
    {A, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, A},
    {N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, C, N, N, N, A},
    {N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, A},
    {N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N},
    {A, N, N, N, N, N, N, N, N, N, N, N, C, N, N, N, N, N, N, N},
    {A, N, N, N, C, N, N, N, N, N, N, N, N, N, N, P, N, N, N, N},
    {A, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, A},
    {A, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, C, N, N, A},
    {A, N, N, P, N, N, N, N, N, N, N, P, N, N, N, N, N, N, N, A},
    {A, N, N, N, N, P, N, N, N, N, N, P, P, N, N, N, N, N, N, A},
    {A, N, N, N, N, N, N, N, N, N, P, P, P, N, N, N, N, N, N, A},
    {A, A, A, A, A, A, N, N, N, A, P, P, P, P, A, A, A, A, A, A},
};

int mapa_decos[MAPA_LINHAS][MAPA_COLUNAS] = {
    {},
    {},
    {DN, DN, DN, DN, DN, DN, DN, DN, DN, DN, DN, DN, DN, DG, DG},
    {DN, DN, DN, DP, DN, DN, DN, DN, DN, DN, DN, DN, DN, DN, DG},
    {DN, DN, DN, DP},
    {DN, DN, DN, DN, DP},
    {DN, DN, DN, DN, DN, DN, DN, DN, DN, DG},
    {DN, DN, DN, DN, DN, DN, DN, DN, DN, DG, DG},
    {DN, DN, DN, DN, DN, DN, DN, DN, DG, DG, DG},
    {DN, DN, DN, DN, DN, DN, DN, DN, DG, DN},
    {DN, DN, DN, DN, DN, DN, DN, DN, DN, DN},
    {},
    {DN, DN, DN, DN, DN, DN, DN, DN, DN, DN, DN, DN, DN, DN, DN, DN, DN},
    {DN, DG, DN, DN, DN, DN, DN, DN, DN, DN, DN, DN, DN, DN, DN, DP, DP},
    {DN, DG, DG, DN, DN, DN, DN, DN, DN, DN, DN, DN, DN, DN, DN, DN, DP, DP},
    {},
};

/*
    Uma função cujo único propósito é redesenhar o cenário.
*/
void desenhar_mapa(FolhaSprites sprites) {
    for (int lin = 0; lin < MAPA_LINHAS; lin++) {
        for (int col = 0; col < MAPA_COLUNAS; col++) {
            int x = col * TAM_BLOCOS;
            int y = lin * TAM_BLOCOS;

            al_draw_scaled_bitmap(sprites.areia, 0, 0, 16, 16, x, y, 48, 48, 0);

            // ----------
            // Blocos
            // ----------
            switch (mapa_blocos[lin][col]) {
            case C:
                al_draw_scaled_bitmap(
                    sprites.sombra, 0, 0, 16, 16, x, y, 48, 48, 0
                );
                al_draw_scaled_bitmap(
                    sprites.cacto, 0, 0, 16, 16, x, y, 48, 48, 0
                );
                break;

            case P:
                al_draw_scaled_bitmap(
                    sprites.sombra, 0, 0, 16, 16, x, y, 48, 48, 0
                );
                al_draw_scaled_bitmap(
                    sprites.pedra, 0, 0, 16, 16, x, y, 48, 48, 0
                );
                break;

            case A:
                al_draw_scaled_bitmap(
                    sprites.sombra, 0, 0, 16, 16, x, y, 48, 48, 0
                );
                al_draw_scaled_bitmap(
                    sprites.arbusto, 0, 0, 16, 16, x, y, 48, 48, 0
                );
                break;
            }

            // ----------
            // Decorações
            // ----------
            switch (mapa_decos[lin][col]) {
            case DG:
                al_draw_scaled_bitmap(
                    sprites.grama, 0, 0, 16, 16, x, y, 48, 48, 0
                );
                break;

            case DP:
                al_draw_scaled_bitmap(
                    sprites.pedrinhas, 0, 0, 16, 16, x, y, 48, 48, 0
                );
                break;
            }
        }
    }
}

/*
    Uma função que recebe um par de coordenadas, o tamanho de uma bounding box
   quadrada e retorna um booleano dizendo se a box toca em alguma peça do
   cenário ou não.
*/
int colide_no_cenario(int x, int y, int tam_box) {
    tam_box /= 2; // Tem que ser sempre a metade pra centralizar
    tam_box -= 1; // Pixelzinho só pra não ficar sempre justo

    int cel_x;
    int cel_y;

    // Superior Esquerdo
    cel_x = (x - tam_box) / TAM_BLOCOS;
    cel_y = (y - tam_box) / TAM_BLOCOS;
    // al_draw_filled_circle(x - tam_box, y - tam_box, 3, al_map_rgb(0, 255,
    // 0));
    if (mapa_blocos[cel_y][cel_x] >= 1) {
        return 1;
    }

    // Superior Direito
    cel_x = (x + tam_box) / TAM_BLOCOS;
    cel_y = (y - tam_box) / TAM_BLOCOS;
    // al_draw_filled_circle(x + tam_box, y - tam_box, 3, al_map_rgb(0, 255,
    // 0));
    if (mapa_blocos[cel_y][cel_x] >= 1) {
        return 1;
    }

    // Inferior Esquerdo
    cel_x = (x - tam_box) / TAM_BLOCOS;
    cel_y = (y + tam_box) / TAM_BLOCOS;
    // al_draw_filled_circle(x - tam_box, y + tam_box, 3, al_map_rgb(0, 255,
    // 0));
    if (mapa_blocos[cel_y][cel_x] >= 1) {
        return 1;
    }

    // Inferior Direito
    cel_x = (x + tam_box) / TAM_BLOCOS;
    cel_y = (y + tam_box) / TAM_BLOCOS;
    // al_draw_filled_circle(x + tam_box, y + tam_box, 3, al_map_rgb(0, 255,
    // 0));
    if (mapa_blocos[cel_y][cel_x] >= 1) {
        return 1;
    }

    return 0;
}

/*
    Calcula a coordenada X e Y absoluta e centralizada de um bloco Xb e Yb.

    Exemplo:
        pegar_coord(1, 2) -> {66, 120}
*/
CoordMapa pegar_coord_centro_bloco(int x_bloco, int y_bloco) {
    return (CoordMapa){
        .x = (x_bloco * TAM_BLOCOS) + TAM_BLOCOS / 2,
        .y = (y_bloco * TAM_BLOCOS) + TAM_BLOCOS / 2,
    };
}