#include "powerups.h"
#include "../constantes.h"
#include "../utils.h"
#include "jogador.h"
#include <allegro5/allegro_primitives.h>

/*
    Renderiza o menu de selecionar powerup.
*/
void desenhar_powerups(EPowerUps powers[3], ALLEGRO_FONT *fonte) {
    int x = LARGURA / 2;
    int y = ALTURA / 2;

    float altu = 80;
    float larg = 600;

    int desvio = 100;

    al_draw_filled_rectangle(
        0, 0, LARGURA, ALTURA, al_map_rgba(20, 20, 20, 150)
    );

    for (int i = 0; i < 3; i++) {
        char desc[100] = "";

        // Eu não vou fazer uma função só pra castar um char
        char mini[2] = "\0\0";

        switch (powers[i]) {
        case AUMENTO_DANO:
            strcat(desc, "Dano +10%");
            break;

        case AUMENTO_VDA:
            strcat(desc, "Cadência +10%");
            break;

        case AUMENTO_VDM:
            strcat(desc, "Velocidade +10%");
            break;

        default:
            strcat(desc, "<POWERUP DESCONHECIDO>");
            break;
        }

        desenhar_caixa_texto(
            desc, COR_BRANCO, x, y - desvio, larg, altu, fonte
        );

        mini[0] = '1' + i;
        desenhar_caixa_texto(
            mini,
            al_map_rgb(255, 233, 150),
            x - (larg / 2),
            y - desvio,
            60,
            60,
            fonte
        );

        desvio -= 100;
    }
}

/*
    Aplica um powerup no jogador.
*/
void aplicar_power(Jogador *canga, EPowerUps power) {
    switch (power) {
    case AUMENTO_DANO:
        canga->dano *= 1.10;
        break;

    case AUMENTO_VDA:
        canga->cooldown_arma *= 0.90;

        if (canga->cooldown_arma < 5) {
            canga->cooldown_arma = 5;
        }
        break;

    case AUMENTO_VDM:
        canga->velocidade *= 1.10;
        break;
    }
}