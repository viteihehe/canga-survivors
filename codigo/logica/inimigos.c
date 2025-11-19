#include "inimigos.h"
#include "../constantes.h"
#include "cenario.h"
#include <allegro5/allegro_primitives.h>
#include <allegro5/color.h>
#include <math.h>

void frames(Inimigo *inimigo) {
    int frame_delay = 10;
    inimigo->contador_frames++;

    if (inimigo->contador_frames >= frame_delay) {
        inimigo->frame_atual++;

        if (inimigo->frame_atual >= inimigo->total_frames) {
            inimigo->frame_atual = 0;
        }

        inimigo->contador_frames = 0;
    }
}

/*
    Função de criar todos os inimigos de maneira compacta
*/
void criarInimigo(
    Inimigo **tatus,
    Inimigo **formigas,
    double *counts,
    ALLEGRO_BITMAP *sprite_formiga,
    ALLEGRO_BITMAP *sprite_tatu,
    double *ultimo_spawn_tatu,
    double *ultimo_spawn_formiga,
    int *indice_tatu,
    int *indice_formiga,
    double *cooldoown_tatu,
    double *cooldoown_formiga,
    int tipo,
    int *contador_total
) {
    int spawn = rand() % 4;
    CoordMapa coords = {};

    switch (spawn) {
    case 0:
        coords = pegar_coord_centro_bloco(9, 0);
        break;

    case 1:
        coords = pegar_coord_centro_bloco(0, 6);
        break;

    case 2:
        coords = pegar_coord_centro_bloco(7, 15);
        break;

    case 3:
        coords = pegar_coord_centro_bloco(19, 8);
        break;
    }

    if (tipo == 0) {
        if (*counts - *ultimo_spawn_tatu >= *cooldoown_tatu &&
            *indice_tatu < 50) {
            Inimigo tatu_temp = {
                .comportamento = TATU,
                .tamanho_box = 20,

                .sprite = sprite_tatu,
                .tamanho_sprite = 64,
                .total_frames = 2,

                .vida = 120,
                .vida_max = 120,
                .dano = 1,
                .velocidade = 1,
                .ativo = true,
                .contador_frames = 0,
            };

            (*indice_tatu)++;
            *tatus = realloc(*tatus, sizeof(Inimigo) * (*indice_tatu + 1));
            (*tatus)[*indice_tatu - 1] = tatu_temp;

            (*tatus)[*indice_tatu - 1].posx = coords.x;
            (*tatus)[*indice_tatu - 1].posy = coords.y;

            *ultimo_spawn_tatu = *counts;
            (*contador_total)++;
        }
    }

    if (tipo == 1) {
        if (*counts - *ultimo_spawn_formiga >= *cooldoown_formiga &&
            *indice_formiga < 100) {

            Inimigo temp_formiga = {
                .comportamento = FORMIGA,
                .tamanho_box = 20,

                .sprite = sprite_formiga,
                .tamanho_sprite = 48,
                .total_frames = 2,

                .vida = 65,
                .vida_max = 65,
                .dano = 1,
                .velocidade = 0.3,
                .ativo = true,
            };

            (*indice_formiga)++;
            *formigas = realloc(*formigas, sizeof(Inimigo) * (*indice_formiga));
            (*formigas)[*indice_formiga - 1] = temp_formiga;

            (*formigas)[*indice_formiga - 1].posx = coords.x;
            (*formigas)[*indice_formiga - 1].posy = coords.y;

            (*contador_total)++;
            *ultimo_spawn_formiga = *counts;
        }
    }
}

void inimigosLogica(
    Inimigo inimigos[], int *indice, Jogador canga, double *counts
) {

    int colisao = 0;
    /*
        O tatu tem um movimento fixo seguindo as coordenadas dos players, se não
       estiver colidindo de uma parede.
    */
    for (int i = 0; i < *indice; i++) {
        if (inimigos[i].comportamento == TATU) {
            colisao = 28;
            if (!inimigos[i].ativo)
                continue;
            int x_futuro = inimigos[i].posx;
            int y_futuro = inimigos[i].posy;

            if (inimigos[i].posx < canga.x) {
                x_futuro += inimigos[i].velocidade;
            }
            if (inimigos[i].posy < canga.y) {
                y_futuro += inimigos[i].velocidade;
            }
            if (inimigos[i].posx > canga.x) {
                x_futuro -= inimigos[i].velocidade;
            }
            if (inimigos[i].posy > canga.y) {
                y_futuro -= inimigos[i].velocidade;
            }
            if (!colide_no_cenario(x_futuro, y_futuro, 22)) {
                inimigos[i].posx = x_futuro;
                inimigos[i].posy = y_futuro;
            }
            // if (!colide_no_cenario((int)x_futuro, (int)inimigos[i].posy, 64))
            // {
            //     inimigos[i].posx = x_futuro;
            // }
            // if (!colide_no_cenario((int)inimigos[i].posx, (int)y_futuro, 64))
            // {
            //     inimigos[i].posy = y_futuro;
            // }
        }

        if (inimigos[i].comportamento == FORMIGA) {
            colisao = 22;
            /*
                A formiga se aproxima do jogador até determinado ponto,
                e foge caso o jogador tente se aproximar.
           */
            if (inimigos[i].ativo) {
                float x = inimigos[i].posx;
                float y = inimigos[i].posy;
                int distancia = 100;

                // aproximação segura
                if (x < canga.x - distancia) {
                    inimigos[i].posx += inimigos[i].velocidade;
                } else if (x > canga.x + distancia) {
                    inimigos[i].posx -= inimigos[i].velocidade;
                }
                if (y < canga.y - distancia) {
                    inimigos[i].posy += inimigos[i].velocidade;
                } else if (y > canga.y + distancia) {
                    inimigos[i].posy -= inimigos[i].velocidade;
                }
                // fuga
                float dx = canga.x - x;
                float dy = canga.y - y;
                if (fabs(dx) < distancia) {
                    if (dx > 0) {
                        inimigos[i].posx -= inimigos[i].velocidade;
                    } else {
                        inimigos[i].posx += inimigos[i].velocidade;
                    }
                }
                if (fabs(dy) < distancia) {
                    if (dy > 0) {
                        inimigos[i].posy -= inimigos[i].velocidade;
                    } else {
                        inimigos[i].posy += inimigos[i].velocidade;
                    }
                }
                if (colide_no_cenario(x, y, 40)) {
                    inimigos[i].posx = x;
                    inimigos[i].posy = y;
                }
            }
        }
    }

    // Isso tá conseguindo chegar como NULL aqui. Não deveria
    if (inimigos == NULL) {
        return;
    }

    colisaoInimigos(inimigos, indice, colisao, inimigos->tamanho_sprite);
}

void colisaoInimigos(
    Inimigo inimigos[], int *indice, int tamanho, int tamanhosprite
) {
    for (int i = 0; i < *indice; i++) {
        if (!inimigos[i].ativo)
            continue;

        if (inimigos[i].vida <= 0) {
            inimigos[i].ativo = 0;
            continue;
        }
        for (int j = i + 1; j < *indice; j++) {
            if (!inimigos[j].ativo)
                continue;
            int colisao_x = tamanho;
            int colisao_y = tamanho;
            if (abs(inimigos[i].posx - inimigos[j].posx) <= colisao_x &&
                abs(inimigos[i].posy - inimigos[j].posy) <= colisao_y) {
                if (inimigos[i].posx < inimigos[j].posx) {
                    inimigos[i].posx -= inimigos[i].velocidade / 1.5;
                    inimigos[j].posx += inimigos[j].velocidade / 1.5;
                } else {
                    inimigos[i].posx += inimigos[i].velocidade / 1.5;
                    inimigos[j].posx -= inimigos[j].velocidade / 1.5;
                }
                if (inimigos[i].posy < inimigos[j].posy) {
                    inimigos[i].posy -= inimigos[i].velocidade / 1.5;
                    inimigos[j].posy += inimigos[j].velocidade / 1.5;
                } else {
                    inimigos[i].posy += inimigos[i].velocidade / 1.5;
                    inimigos[j].posy -= inimigos[j].velocidade / 1.5;
                }
            }
        }
    }
}

void colisaoBala(
    Bala *bala_atual, Inimigo *inimigo_atual, int colisao, Som som
) {
    if (bala_atual->ativa && inimigo_atual->ativo) {
        if (abs(bala_atual->x - inimigo_atual->posx) < colisao &&
            abs(bala_atual->y - inimigo_atual->posy) < colisao) {
            inimigo_atual->vida -= bala_atual->dano;
            bala_atual->ativa = false;
            al_play_sample(som.hit_inimigo, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
        }
    }
}

void processamentoBala(
    Inimigo inimigos[],
    int *indice,
    Bala balas[],
    int *max_balas,
    int colisao,
    Jogador *canga,
    Som *sons,
    int *contador_morte
) {
    for (int i = 0; i < *indice; i++) {
        if (!inimigos[i].ativo)
            continue;
        for (int j = 0; j < *max_balas; j++) {
            if (!balas[j].ativa)
                continue;

            colisaoBala(&balas[j], &inimigos[i], colisao, *sons);
            if (!balas[j].ativa) {
                if (inimigos[i].vida <= 0) {
                    inimigos[i].ativo = false;
                    (*contador_morte)++;
                    al_play_sample(
                        sons->morte_inimigos,
                        0.5,
                        0,
                        1,
                        ALLEGRO_PLAYMODE_ONCE,
                        0
                    );
                }
                break;
            }
        }
    }
    reajusteInimigos(inimigos, indice);
}

void reajusteInimigos(Inimigo inimigos[], int *indice) {
    int vivos = 0;
    for (int i = 0; i < *indice; i++) {
        if (inimigos[i].ativo) {
            inimigos[vivos++] = inimigos[i];
        }
    }
    *indice = vivos;
}

void desenharInimigo(Inimigo inimigos[], int indice, Jogador canga) {
    for (int i = 0; i < indice; i++) {
        if (inimigos[i].ativo) {
            frames(&inimigos[i]);
            int png_x = inimigos[i].frame_atual * inimigos[i].tamanho_sprite;
            int png_y = 0;
            int flip = 0;
            if (inimigos[i].posx < canga.x) {
                flip = ALLEGRO_FLIP_HORIZONTAL;
            }

            al_draw_bitmap_region(
                inimigos[i].sprite,
                png_x,
                png_y,
                inimigos[i].tamanho_sprite,
                inimigos[i].tamanho_sprite,
                inimigos[i].posx - inimigos[i].tamanho_sprite / 2.0,
                inimigos[i].posy - inimigos[i].tamanho_sprite / 2.0,
                flip
            );
        }
    }
}

void danoJogador(
    Inimigo inimigos[], Jogador *canga, int indice, double counts, Som som
) {
    for (int i = 0; i < indice; i++) {
        if (!inimigos[i].ativo)
            continue;
        if (inimigos[i].comportamento == TATU) {
            int colisaox = 40;
            int colisaoy = 40;
            if ((abs(inimigos[i].posx - canga->x) < colisaox) &&
                (abs(inimigos[i].posy - canga->y) < colisaoy) &&
                counts - canga->ultimo_dano >= canga->dano_delay) {
                canga->vida -= 1;
                canga->ultimo_dano = counts;
                al_play_sample(som.hit, 2, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
            }
        }
    }

    if (canga->vida <= 0)
        canga->vivo = false;
}

/*
    Redesenha a barra de vida acima das cabeças dos inimigos.
*/
void desenhar_vida_inimigos(Inimigo *inimigos, int quant_inimigos) {
    for (int i = 0; i < quant_inimigos; i++) {
        int desvio = inimigos[i].tamanho_sprite / 2;

        // Contorno
        al_draw_filled_rectangle(
            inimigos[i].posx - desvio - 1,
            inimigos[i].posy - 45 - 1,
            inimigos[i].posx + desvio + 1,
            inimigos[i].posy - 40 + 1,
            COR_BRANCO
        );

        // Fundo
        al_draw_filled_rectangle(
            inimigos[i].posx - desvio,
            inimigos[i].posy - 45,
            inimigos[i].posx + desvio,
            inimigos[i].posy - 40,
            COR_PRETO
        );

        // Vida
        float proporcao_vida = (float)inimigos[i].vida / inimigos[i].vida_max;

        al_draw_filled_rectangle(
            inimigos[i].posx - desvio,
            inimigos[i].posy - 45,
            inimigos[i].posx - desvio +
                (inimigos[i].tamanho_sprite * proporcao_vida),
            inimigos[i].posy - 40,
            al_map_rgb(255, 0, 0)
        );
    }
};
