#include "jogador.h"
#include "../constantes.h"
#include "cenario.h"
#include <allegro5/allegro_primitives.h>

/*
    Uma função cujo propósito é atualizar o estado das teclas WASD do jogador.
*/
void capturar_movimento(ALLEGRO_EVENT evento, MapaDirecoes *teclas) {
    if (evento.type == ALLEGRO_EVENT_KEY_DOWN) {
        switch (evento.keyboard.keycode) {
        case ALLEGRO_KEY_W:
            teclas->cima = true;
            break;

        case ALLEGRO_KEY_S:
            teclas->baixo = true;
            break;

        case ALLEGRO_KEY_A:
            teclas->esq = true;
            break;

        case ALLEGRO_KEY_D:
            teclas->dir = true;
            break;
        }
    }

    if (evento.type == ALLEGRO_EVENT_KEY_UP) {
        switch (evento.keyboard.keycode) {
        case ALLEGRO_KEY_W:
            teclas->cima = false;
            break;

        case ALLEGRO_KEY_S:
            teclas->baixo = false;
            break;

        case ALLEGRO_KEY_A:
            teclas->esq = false;
            break;

        case ALLEGRO_KEY_D:
            teclas->dir = false;
            break;
        }
    }
}

/*
    Uma função cujo propósito é atualizar o estado das teclas seta do jogador.
*/
void capturar_mira(ALLEGRO_EVENT evento, MapaDirecoes *teclas) {
    if (evento.type == ALLEGRO_EVENT_KEY_DOWN) {
        switch (evento.keyboard.keycode) {
        case ALLEGRO_KEY_UP:
            teclas->cima = true;
            break;

        case ALLEGRO_KEY_DOWN:
            teclas->baixo = true;
            break;

        case ALLEGRO_KEY_LEFT:
            teclas->esq = true;
            break;

        case ALLEGRO_KEY_RIGHT:
            teclas->dir = true;
            break;
        }
    }

    if (evento.type == ALLEGRO_EVENT_KEY_UP) {
        switch (evento.keyboard.keycode) {
        case ALLEGRO_KEY_UP:
            teclas->cima = false;
            break;

        case ALLEGRO_KEY_DOWN:
            teclas->baixo = false;
            break;

        case ALLEGRO_KEY_LEFT:
            teclas->esq = false;
            break;

        case ALLEGRO_KEY_RIGHT:
            teclas->dir = false;
            break;
        }
    }
}

/*
    Uma função cujo propósito é atualizar e redesenhar, se possível, a posição
   do jogador na tela.

   TODO: Mover a parte do redesenho para uma função dedicada.
*/
void mover_jogador(MapaDirecoes teclas, Jogador *jogador) {
    int x_futuro = jogador->x;
    int y_futuro = jogador->y;

    // Calculando a próxima posição
    if (teclas.cima && jogador->y > 0) {
        y_futuro -= jogador->velocidade;
    }

    if (teclas.baixo && jogador->y < ALTURA) {
        y_futuro += jogador->velocidade;
    }

    if (teclas.esq && jogador->x > 0) {
        x_futuro -= jogador->velocidade;
    }

    if (teclas.dir && jogador->x < LARGURA) {
        x_futuro += jogador->velocidade;
    }

    // Checando se dá pra mover
    if (!colide_no_cenario(jogador->x, y_futuro, 40)) {
        jogador->y = y_futuro;
    }

    if (!colide_no_cenario(x_futuro, jogador->y, 40)) {
        jogador->x = x_futuro;
    }

    al_draw_bitmap(
        jogador->sprite,
        jogador->x - 32,
        jogador->y - 32,
        ALLEGRO_FLIP_HORIZONTAL
    );
}

/*
    Uma função cujo propósito é gerar adicionar, se possível, uma nova bala do
   jogador na memória e atualizar o timestamp do último tiro.

    A função também exporta a quantidade de balas para fora por meio do
   argumento `dest_quant`.
*/
void criar_bala_jogador(
    Bala **balas,
    int *dest_quant,
    Jogador *jogador,
    ALLEGRO_TIMER *tick_timer,
    FolhaSprites sprites,
    Som som
) {
    // O jogador tem que estar mirando em alguma direção
    if (!(jogador->mira.cima || jogador->mira.baixo || jogador->mira.esq ||
          jogador->mira.dir)) {
        return;
    }

    // Não pode estar mirando em direções opostas
    if ((jogador->mira.cima && jogador->mira.baixo) ||
        (jogador->mira.esq && jogador->mira.dir)) {
        return;
    }

    // Arma não pode estar resfriando
    if (al_get_timer_count(tick_timer) < jogador->tempo_ultimo_disparo) {
        return;
    }

    Bala bala_temp = {
        sprites.bala, jogador->x, jogador->y, jogador->mira, true, jogador->dano
    };

    (*dest_quant)++;
    *balas = realloc(*balas, sizeof(Bala) * *dest_quant);
    (*balas)[*dest_quant - 1] = bala_temp;

    jogador->tempo_ultimo_disparo =
        al_get_timer_count(tick_timer) + jogador->cooldown_arma;
    al_play_sample(som.disparo, 1.5, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
}

/*
    Uma função cujo propósito é atualizar a posição das balas e redesenhar todo
   tick.

    TODO: Mover a parte de redesenhar para uma função dedicada;
*/
void mover_balas(Bala *balas, int quant_balas) {
    for (int i = 0; i < quant_balas; i++) {
        if (!balas[i].ativa) {
            continue;
        }

        if (balas[i].direcoes.cima) {
            balas[i].y -= VEL_BALA;
        }

        if (balas[i].direcoes.baixo) {
            balas[i].y += VEL_BALA;
        }

        if (balas[i].direcoes.esq) {
            balas[i].x -= VEL_BALA;
        }

        if (balas[i].direcoes.dir) {
            balas[i].x += VEL_BALA;
        }

        if (colide_no_cenario(balas[i].x, balas[i].y, 12)) {
            balas[i].ativa = false;
            return;
        }

        al_draw_bitmap(
            balas[i].sprite,
            balas[i].x - 8,
            balas[i].y - 8,
            ALLEGRO_FLIP_HORIZONTAL
        );
    }
}

/*
    Redesenha as vidas em cima do canga.
*/
void desenhar_vida_jogador(Jogador *canga, FolhaSprites sprites) {
    float inicial = canga->x - (8 * canga->vida);

    for (int i = 0; i < canga->vida; i++) {
        al_draw_bitmap(sprites.coracao, inicial + (i * 16), canga->y - 50, 0);
    }
}