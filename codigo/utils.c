#include "utils.h"
#include <allegro5/allegro_primitives.h>

/*
    Desenha uma caixa de texto centralizada em (X, Y). Tamanho de fonte
   recomendado: 22.
*/
void desenhar_caixa_texto(char *texto, ALLEGRO_COLOR cor, int x, int y,
                          float larg, float altu, ALLEGRO_FONT *fonte) {
    float desvio_x = larg / 2;
    float desvio_y = altu / 2;

    al_draw_filled_rectangle(x - desvio_x, y - desvio_y, x + desvio_x,
                             y + desvio_y, al_map_rgb(0, 0, 0));

    al_draw_rectangle(x - desvio_x, y - desvio_y, x + desvio_x, y + desvio_y,
                      al_map_rgb(255, 255, 255), 5);

    al_draw_text(fonte, cor, x - 2, y - 8, ALLEGRO_ALIGN_CENTER, texto);
}