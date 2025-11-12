#include "utils.h"
#include "constantes.h"
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>

/*
    Desenha uma caixa de texto centralizada em (X, Y). Tamanho de fonte
   recomendado: 22.
*/
void desenhar_caixa_texto(
    char *texto,
    ALLEGRO_COLOR cor,
    int x,
    int y,
    float larg,
    float altu,
    ALLEGRO_FONT *fonte
) {
    float desvio_x = larg / 2;
    float desvio_y = altu / 2;

    // Preenchimento
    al_draw_filled_rectangle(
        x - desvio_x, y - desvio_y, x + desvio_x, y + desvio_y, COR_PRETO
    );

    // Bordas
    al_draw_rectangle(
        x - desvio_x, y - desvio_y, x + desvio_x, y + desvio_y, COR_BRANCO, 5
    );

    al_draw_text(
        fonte,
        cor,
        x - 2,
        y - (al_get_font_ascent(fonte) / 2.0),
        ALLEGRO_ALIGN_CENTER,
        texto
    );
}

/*
    Atualiza o `indice` ciclicamente como base no mínimo, máximo e no
   incremento.
*/
void ciclar_inteiro(int *indice, int min, int max, int incremento) {
    *indice += incremento;

    if (*indice < min) {
        *indice = max;
    }

    if (*indice > max) {
        *indice = min;
    }
}