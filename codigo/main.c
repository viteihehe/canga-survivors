#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/bitmap.h>
#include <allegro5/bitmap_draw.h>
#include <allegro5/bitmap_io.h>
#include <allegro5/color.h>
#include <allegro5/display.h>
#include <allegro5/events.h>
#include <allegro5/keyboard.h>
#include <allegro5/keycodes.h>
#include <allegro5/mouse.h>
#include <allegro5/system.h>
#include <allegro5/timer.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define VEL_JOGADOR 3 // Em pixels
#define VEL_BALA 10   // Em frames

#define VELOCIDADE 3
#define LARGURA 960
#define ALTURA 768
#define FPS 60

#define MAPA_LINHAS 16
#define MAPA_COLUNAS 20
#define TAM_BLOCOS 48

typedef enum { CIMA, BAIXO, DIREITA, ESQUERDA } Direcoes;
typedef enum {
    TATU,
    FORMIGA,
} Ecomportamento;

enum EBloco {
    N, // Nada
    C, // Cacto
    P, // Pedra
    A, // Arbusto
};

int mapa_inicial[MAPA_LINHAS][MAPA_COLUNAS] = {
    {A, A, A, A, A, A, A, A, A, A, A, A, A, A, A, A, P, P, P, P},
    {A, N, N, N, P, P, N, N, N, N, N, N, N, N, N, N, P, P, P, P},
    {A, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, P, P},
    {A, N, C, N, N, N, N, N, C, N, N, N, N, N, N, N, N, N, N, A},
    {A, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, A},
    {A, N, N, N, N, N, N, N, N, N, N, N, N, N, N, C, N, N, N, A},
    {A, N, N, C, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, A},
    {A, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, P, N, A},
    {A, N, N, N, N, N, N, N, N, N, N, N, C, N, N, N, N, N, N, A},
    {A, N, N, N, C, N, N, N, N, N, N, N, N, N, N, N, N, N, N, A},
    {A, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, A},
    {A, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, C, N, N, A},
    {A, N, P, P, N, N, N, N, N, N, N, P, N, N, N, N, N, N, N, A},
    {A, N, P, N, N, N, P, N, N, N, N, P, P, N, N, N, N, N, N, A},
    {A, N, N, N, N, N, N, N, N, N, P, P, P, N, N, N, N, N, N, A},
    {A, A, A, A, A, A, A, A, A, A, P, P, P, P, A, A, A, A, A, A},
};

typedef struct {
    ALLEGRO_BITMAP *canga;

    ALLEGRO_BITMAP *areia;
    ALLEGRO_BITMAP *cacto;
    ALLEGRO_BITMAP *pedra;
    ALLEGRO_BITMAP *arbusto;

    ALLEGRO_BITMAP *sombra;
    ALLEGRO_BITMAP *bala;
} FolhaSprites;

/*
    Uma função cujo único propósito é redesenhar o cenário.
*/
void redesenhar_mapa(FolhaSprites sprites) {
    for (int lin = 0; lin < MAPA_LINHAS; lin++) {
        for (int col = 0; col < MAPA_COLUNAS; col++) {
            int x = col * TAM_BLOCOS;
            int y = lin * TAM_BLOCOS;

            al_draw_scaled_bitmap(sprites.areia, 0, 0, 16, 16, x, y, 48, 48, 0);

            switch (mapa_inicial[lin][col]) {
            case N:
                // Esse case só serve pra o bloco vazio não cair no default
                break;

            case C:
                al_draw_scaled_bitmap(sprites.sombra, 0, 0, 16, 16, x, y, 48,
                                      48, 0);
                al_draw_scaled_bitmap(sprites.cacto, 0, 0, 16, 16, x, y, 48, 48,
                                      0);
                break;

            case P:
                al_draw_scaled_bitmap(sprites.sombra, 0, 0, 16, 16, x, y, 48,
                                      48, 0);
                al_draw_scaled_bitmap(sprites.pedra, 0, 0, 16, 16, x, y, 48, 48,
                                      0);
                break;

            case A:
                al_draw_scaled_bitmap(sprites.sombra, 0, 0, 16, 16, x, y, 48,
                                      48, 0);
                al_draw_scaled_bitmap(sprites.arbusto, 0, 0, 16, 16, x, y, 48,
                                      48, 0);
                break;

            default:
                al_draw_filled_rectangle(x, y, x + TAM_BLOCOS, y + TAM_BLOCOS,
                                         al_map_rgb(199, 36, 147));
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
    if (mapa_inicial[cel_y][cel_x] >= 1) {
        return 1;
    }

    // Superior Direito
    cel_x = (x + tam_box) / TAM_BLOCOS;
    cel_y = (y - tam_box) / TAM_BLOCOS;
    // al_draw_filled_circle(x + tam_box, y - tam_box, 3, al_map_rgb(0, 255,
    // 0));
    if (mapa_inicial[cel_y][cel_x] >= 1) {
        return 1;
    }

    // Inferior Esquerdo
    cel_x = (x - tam_box) / TAM_BLOCOS;
    cel_y = (y + tam_box) / TAM_BLOCOS;
    // al_draw_filled_circle(x - tam_box, y + tam_box, 3, al_map_rgb(0, 255,
    // 0));
    if (mapa_inicial[cel_y][cel_x] >= 1) {
        return 1;
    }

    // Inferior Direito
    cel_x = (x + tam_box) / TAM_BLOCOS;
    cel_y = (y + tam_box) / TAM_BLOCOS;
    // al_draw_filled_circle(x + tam_box, y + tam_box, 3, al_map_rgb(0, 255,
    // 0));
    if (mapa_inicial[cel_y][cel_x] >= 1) {
        return 1;
    }

    return 0;
}

typedef struct {
    bool cima;
    bool baixo;
    bool esq;
    bool dir;
} MapaDirecoes;

typedef struct {
    int tempo_resfriamento;
    long tempo_ultimo_disparo;
} Arma;

typedef struct {
    ALLEGRO_BITMAP *sprite;
    int x;
    int y;
    MapaDirecoes direcoes;
    bool ativa;
    int dano;
} Bala;

typedef struct {
    ALLEGRO_BITMAP *sprite;
    int x;
    int y;
    MapaDirecoes movimento;
    MapaDirecoes mira;
    Arma arma;
} Jogador;

typedef struct {
    int movimento;
    int posx;
    int posy;
    ALLEGRO_BITMAP *cuspe;
    bool ativa;
} BalaInimigo;

typedef struct {
    Ecomportamento comportamento;
    int tamanho_box;
    int ultimo_ataque;
    int posx;
    int posy;
    ALLEGRO_BITMAP *sprite;
    int vida;
    int dano;
    float velocidade;
    bool ativo;
    int total_frames;
    int frame_atual;
    int quantidade_de_ataques;
    int tamanho_sprite;
} Inimigo;

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
        y_futuro -= VEL_JOGADOR;
    }

    if (teclas.baixo && jogador->y < ALTURA) {
        y_futuro += VEL_JOGADOR;
    }

    if (teclas.esq && jogador->x > 0) {
        x_futuro -= VEL_JOGADOR;
    }

    if (teclas.dir && jogador->x < LARGURA) {
        x_futuro += VEL_JOGADOR;
    }

    // Checando se dá pra mover
    if (!colide_no_cenario(jogador->x, y_futuro, 40)) {
        jogador->y = y_futuro;
    }

    if (!colide_no_cenario(x_futuro, jogador->y, 40)) {
        jogador->x = x_futuro;
    }

    al_draw_bitmap(jogador->sprite, jogador->x - 32, jogador->y - 32,
                   ALLEGRO_FLIP_HORIZONTAL);
}

/*
    Uma função cujo propósito é gerar adicionar, se possível, uma nova bala do
   jogador na memória e atualizar o timestamp do último tiro.

    A função também exporta a quantidade de balas para fora por meio do
   argumento `dest_quant`.
*/
void criar_bala_jogador(Bala **balas, int *dest_quant, Jogador *jogador,
                        ALLEGRO_TIMER *tick_timer, FolhaSprites sprites) {
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
    if (al_get_timer_count(tick_timer) < jogador->arma.tempo_ultimo_disparo) {
        return;
    }

    Bala bala_temp = {sprites.bala, jogador->x, jogador->y, jogador->mira,
                      true};

    (*dest_quant)++;
    *balas = realloc(*balas, sizeof(Bala) * *dest_quant);
    (*balas)[*dest_quant - 1] = bala_temp;

    jogador->arma.tempo_ultimo_disparo =
        al_get_timer_count(tick_timer) + jogador->arma.tempo_resfriamento;
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

        al_draw_bitmap(balas[i].sprite, balas[i].x - 8, balas[i].y - 8,
                       ALLEGRO_FLIP_HORIZONTAL);
    }
}

void frames(int *contador_frames, Inimigo *inimigo) {
    int frame_delay = 10;
    (*contador_frames)++;

    if (*contador_frames >= frame_delay) {
        inimigo->frame_atual++;

        if (inimigo->frame_atual >= inimigo->total_frames) {
            inimigo->frame_atual = 0;
        }

        *contador_frames = 0;
    }
}

void criarInimigo(Inimigo tatus[], Inimigo formigas[], double *counts,
                  ALLEGRO_BITMAP *sprite_formiga, ALLEGRO_BITMAP *sprite_tatu,
                  int *ultimo_spawn_tatu, int *ultimo_spawn_formiga,
                  int *indice_tatu, int *indice_formiga) {
    int cooldoown_tatu = 2;
    int cooldoown_formiga = 4;

    if (*counts - *ultimo_spawn_tatu >= cooldoown_tatu && *indice_tatu < 50) {

        Inimigo tatu_temp = {TATU, 28, 0,    0, 0, sprite_tatu, 3,
                             1,    1,  true, 2, 0, 0,           64};

        tatus[*indice_tatu] = tatu_temp;

        int spawn = rand() % 4;

        switch (spawn) {
        case 0:
            tatus[*indice_tatu].posx = 80;
            tatus[*indice_tatu].posy = ALTURA / 2;
            break;
        case 1:
            tatus[*indice_tatu].posx = LARGURA - 80;
            tatus[*indice_tatu].posy = ALTURA / 2;
            break;
        case 2:
            tatus[*indice_tatu].posx = LARGURA / 2;
            tatus[*indice_tatu].posy = ALTURA - 660;
            break;
        case 3:
            tatus[*indice_tatu].posx = LARGURA / 2;
            tatus[*indice_tatu].posy = ALTURA - 80;
            break;

        default:
            break;
        }

        *ultimo_spawn_tatu = *counts;
        (*indice_tatu)++;
    }
    if (*counts - *ultimo_spawn_formiga >= cooldoown_formiga &&
        *indice_formiga < 100) {

        Inimigo temp_formiga = {FORMIGA, 22, 0, 0, 0, sprite_formiga, 2, 1, 0.5,
                                true,    2,  0, 0, 48};

        formigas[*indice_formiga] = temp_formiga;

        int spawn = rand() % 4;

        switch (spawn) {
        case 0:
            formigas[*indice_formiga].posx = 80;
            formigas[*indice_formiga].posy = ALTURA / 2;
            break;
        case 1:
            formigas[*indice_formiga].posx = LARGURA - 80;
            formigas[*indice_formiga].posy = ALTURA / 2;
            break;
        case 2:
            formigas[*indice_formiga].posx = LARGURA / 2;
            formigas[*indice_formiga].posy = ALTURA - 660;
            break;
        case 3:
            formigas[*indice_formiga].posx = LARGURA / 2;
            formigas[*indice_formiga].posy = ALTURA - 80;
            break;

        default:
            break;
        }

        *ultimo_spawn_formiga = *counts;
        (*indice_formiga)++;
    }
}

void colisaoInimigos(Inimigo inimigos[], int *indice, int tamanho,
                     int tamanhosprite) {
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
            int old_x = inimigos[i].posx;
            int old_y = inimigos[i].posy;
            int old_x2 = inimigos[j].posx;
            int old_y2 = inimigos[j].posy;
            int colisao_x = tamanho;
            int colisao_y = tamanho;
            if (abs(inimigos[i].posx - inimigos[j].posx) <= colisao_x &&
                abs(inimigos[i].posy - inimigos[j].posy) <= colisao_y) {
                if (inimigos[i].posx < inimigos[j].posx) {
                    inimigos[i].posx -= inimigos[i].velocidade;
                    inimigos[j].posx += inimigos[j].velocidade;
                } else {
                    inimigos[i].posx += inimigos[i].velocidade;
                    inimigos[j].posx -= inimigos[j].velocidade;
                }
                if (inimigos[i].posy < inimigos[j].posy) {
                    inimigos[i].posy -= inimigos[i].velocidade;
                    inimigos[j].posy += inimigos[j].velocidade;
                } else {
                    inimigos[i].posy += inimigos[i].velocidade;
                    inimigos[j].posy -= inimigos[j].velocidade;
                }
                if (colide_no_cenario(inimigos[i].posx, inimigos[i].posy,
                                      tamanhosprite)) {
                    inimigos[i].posx = old_x;
                    inimigos[i].posy = old_y;
                }
                if (colide_no_cenario(inimigos[j].posx, inimigos[j].posy,
                                      tamanhosprite)) {
                    inimigos[j].posx = old_x2;
                    inimigos[j].posy = old_y2;
                }
            }
        }
    }
}

void compactarBalaFormiga(BalaInimigo formiga_bala[], int *cont_disparo) {
    int bala_ativa = 0;
    for (int i = 0; i < *cont_disparo; i++) {
        if (formiga_bala[i].ativa) {
            formiga_bala[bala_ativa++] = formiga_bala[i];
        }
    }
    *cont_disparo = bala_ativa;
}

void logicaBalaFormiga(BalaInimigo formiga_bala[], int *cont_disparo) {
    int velocidade_bala = 3;
    for (int i = 0; i < *cont_disparo; i++) {
        if (formiga_bala[i].ativa) {
            if (formiga_bala[i].movimento == CIMA) {
                formiga_bala[i].posy -= velocidade_bala;
            } else if (formiga_bala[i].movimento == BAIXO) {
                formiga_bala[i].posy += velocidade_bala;
            }
            if (formiga_bala[i].movimento == DIREITA) {
                formiga_bala[i].posx += velocidade_bala;
            } else if (formiga_bala[i].movimento == ESQUERDA) {
                formiga_bala[i].posx -= velocidade_bala;
            }

            if (formiga_bala[i].posx < 0 || formiga_bala[i].posx > LARGURA ||
                formiga_bala[i].posy < 0 || formiga_bala[i].posy > ALTURA) {
                formiga_bala[i].ativa = false;
            }
        }
        if (colide_no_cenario(formiga_bala[i].posx, formiga_bala[i].posy, 12)) {
            formiga_bala[i].ativa = 0;
        }
    }
    compactarBalaFormiga(formiga_bala, cont_disparo);
}

void inimigosLogica(Inimigo inimigos[], int *indice, Jogador canga,
                    double *counts, BalaInimigo formiga_bala[],
                    ALLEGRO_BITMAP *cuspe) {
    const int disparo_cooldown = 2;
    int colisao;
    for (int i = 0; i < *indice; i++) {
        if (inimigos[i].comportamento == TATU) {
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
            if (!colide_no_cenario(x_futuro, inimigos[i].posy, 64)) {
                inimigos[i].posx = x_futuro;
            }
            if (!colide_no_cenario(inimigos[i].posx, y_futuro, 64)) {
                inimigos[i].posy = y_futuro;
            }
            colisao = 24;
        }

        if (inimigos[i].comportamento == FORMIGA) {
            colisao = 22;
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
                if (colide_no_cenario(inimigos[i].posx, inimigos[i].posy, 48)) {
                    inimigos[i].posx = x;
                    inimigos[i].posy = y;
                }

                if (*counts - inimigos[i].ultimo_ataque >= disparo_cooldown &&
                    inimigos[i].quantidade_de_ataques < 2000) {
                    BalaInimigo *bala_formiga =
                        &formiga_bala[inimigos[i].quantidade_de_ataques];
                    bala_formiga->cuspe = cuspe;
                    bala_formiga->ativa = true;

                    if (canga.x < inimigos[i].posx - 48) {
                        bala_formiga->movimento = ESQUERDA;
                    } else if (canga.x > inimigos[i].posx + 48) {
                        bala_formiga->movimento = DIREITA;
                    } else if (canga.y > inimigos[i].posy - 48) {
                        bala_formiga->movimento = BAIXO;
                    } else if (canga.y < inimigos[i].posy - 48) {
                        bala_formiga->movimento = CIMA;
                    }

                    bala_formiga->posx = inimigos[i].posx + 30;
                    bala_formiga->posy = inimigos[i].posy + 20;
                    inimigos[i].ultimo_ataque = *counts;
                    inimigos[i].quantidade_de_ataques++;
                }
                logicaBalaFormiga(formiga_bala,
                                  &inimigos[i].quantidade_de_ataques);
            }
        }
    }

    colisaoInimigos(inimigos, indice, colisao, inimigos->tamanho_sprite);
}

bool verificaColisao(Bala *bala, Inimigo *inimigo, int colisao) {
    if (abs(bala->x - inimigo->posx) < colisao &&
        abs(bala->y - inimigo->posy) < colisao) {
        return true;
    }
    return false;
}

void colisaoBalaInimigo(Bala *bala_atual, Inimigo *inimigo_atual, int colisao) {
    if (bala_atual->ativa && inimigo_atual->ativo) {
        if (verificaColisao(bala_atual, inimigo_atual, colisao)) {
            inimigo_atual->vida -= bala_atual->dano;
            bala_atual->ativa = false;
        }
    }
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

void processamentoBalaInimigo(Inimigo inimigos[], int *indice, Bala balas[],
                              int *max_balas, int colisao) {
    for (int i = 0; i < *indice; i++) {
        if (!inimigos[i].ativo)
            continue;
        for (int j = 0; j < *max_balas; j++) {
            if (!balas[j].ativa)
                continue;

            colisaoBalaInimigo(&balas[j], &inimigos[i], colisao);
            if (!balas[j].ativa) {
                if (inimigos[i].vida <= 0) {
                    inimigos[i].ativo = false;
                }
                break;
            }
        }
    }
    reajusteInimigos(inimigos, indice);
}

void desenharInimigo(Inimigo inimigos[], int indice, int *contador_frames,
                     Jogador canga, BalaInimigo formiga_bala[]) {
    for (int i = 0; i < indice; i++) {
        if (inimigos[i].ativo) {
            frames(contador_frames, &inimigos[i]);
            int png_x = inimigos[i].frame_atual * inimigos[i].tamanho_sprite;
            int png_y = 0;
            int flip = 0;
            if (inimigos[i].posx < canga.x) {
                flip = ALLEGRO_FLIP_HORIZONTAL;
            }

            // al_draw_bitmap_region(formigas[i].sprite, png_x, png_y, 48, 48,
            //                       formigas[i].posx - 24, formigas[i].posy -
            //                       24, flip);
            al_draw_bitmap_region(
                inimigos[i].sprite, png_x, png_y, inimigos[i].tamanho_sprite,
                inimigos[i].tamanho_sprite,
                inimigos[i].posx - inimigos[i].tamanho_sprite / 2.0,
                inimigos[i].posy - inimigos[i].tamanho_sprite / 2.0, flip);

            if (inimigos[i].comportamento == FORMIGA) {
                for (int j = 0; j < inimigos[i].quantidade_de_ataques; j++) {
                    if (formiga_bala[j].ativa) {
                        al_draw_bitmap(formiga_bala[j].cuspe,
                                       formiga_bala[j].posx - 8,
                                       formiga_bala[j].posy - 8, 0);
                    }
                }
            }
        }
    }
}

int main() {
    // ----------
    // Inicialização
    // ----------
    al_init();
    al_init_image_addon();
    al_init_font_addon();
    al_init_ttf_addon();
    al_init_native_dialog_addon();
    al_init_primitives_addon();
    al_install_keyboard();

    ALLEGRO_DISPLAY *tela = al_create_display(LARGURA, ALTURA);
    ALLEGRO_EVENT_QUEUE *fila = al_create_event_queue();
    al_register_event_source(fila, al_get_keyboard_event_source());
    al_register_event_source(fila, al_get_display_event_source(tela));

    ALLEGRO_TIMER *tick_timer = al_create_timer(1.0 / FPS);
    al_register_event_source(fila, al_get_timer_event_source(tick_timer));
    al_start_timer(tick_timer);

    // ----------
    // Sprites
    // ----------
    FolhaSprites sprites = {
        al_load_bitmap("./materiais/sprites/canga.png"),
        al_load_bitmap("./materiais/sprites/mapa/areia.png"),
        al_load_bitmap("./materiais/sprites/mapa/cacto.png"),
        al_load_bitmap("./materiais/sprites/mapa/pedra.png"),
        al_load_bitmap("./materiais/sprites/mapa/arbusto.png"),
        al_load_bitmap("./materiais/sprites/sombra.png"),
        al_load_bitmap("./materiais/sprites/bala.png")};

    // ----------
    // Jogador
    // ----------
    Jogador canga = {sprites.canga,
                     LARGURA / 2,
                     ALTURA / 2,
                     {false, false, false, false},
                     {false, false, false, false},
                     {30, 0}};

    int quant_balas = 0;
    Bala *balas = NULL;

    //---------
    // Inimigos
    //---------
    int indice_tatu = 0;
    int indice_formiga = 0;
    int contador_frames = 0;
    double counts = 0;
    // Homem Tatu
    Inimigo *homem_tatus = (Inimigo *)malloc(50 * sizeof(Inimigo));
    ALLEGRO_BITMAP *sprite_tatu =
        al_load_bitmap("./materiais/sprites/peba2_1.png");
    int ultimo_spawn_tatu = 0;

    // Formiga
    BalaInimigo *formiga_bala =
        (BalaInimigo *)malloc(2000 * sizeof(BalaInimigo));
    Inimigo *formigas = (Inimigo *)malloc(50 * sizeof(Inimigo));
    ALLEGRO_BITMAP *cuspe = al_load_bitmap("./materiais/sprites/cuspe.png");
    ALLEGRO_BITMAP *sprite_formiga =
        al_load_bitmap("./materiais/sprites/formiga2.png");

    int ultimo_spawn_formiga = 0;

    // ----------
    // Loop Principal
    // ----------
    ALLEGRO_EVENT evento;
    for (;;) {
        al_wait_for_event(fila, &evento);

        capturar_movimento(evento, &canga.movimento);
        capturar_mira(evento, &canga.mira);

        if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            break;
        }

        if (evento.type == ALLEGRO_EVENT_TIMER) {
            criar_bala_jogador(&balas, &quant_balas, &canga, tick_timer,
                               sprites);

            //--------
            // Inimigos
            //--------
            counts = al_get_time();
            criarInimigo(homem_tatus, formigas, &counts, sprite_formiga,
                         sprite_tatu, &ultimo_spawn_tatu, &ultimo_spawn_formiga,
                         &indice_tatu, &indice_formiga);

            inimigosLogica(homem_tatus, &indice_tatu, canga, &counts,
                           formiga_bala, cuspe);
            inimigosLogica(formigas, &indice_formiga, canga, &counts,
                           formiga_bala, cuspe);
            processamentoBalaInimigo(homem_tatus, &indice_tatu, balas,
                                     &quant_balas, 28);
            processamentoBalaInimigo(formigas, &indice_formiga, balas,
                                     &quant_balas, 22);

            // ----------
            // Frames
            // ----------
            // al_draw_bitmap(cenario, 0, 0, ALLEGRO_FLIP_HORIZONTAL);
            al_draw_filled_rectangle(0, 0, LARGURA, ALTURA,
                                     al_map_rgb(0, 0, 0));
            redesenhar_mapa(sprites);

            mover_jogador(canga.movimento, &canga);
            desenharInimigo(homem_tatus, indice_tatu, &contador_frames, canga,
                            formiga_bala);
            desenharInimigo(formigas, indice_formiga, &contador_frames, canga,
                            formiga_bala);
            mover_balas(balas, quant_balas);
            // al_draw_filled_circle(canga.x, canga.y, 5, al_map_rgb(255, 0,
            // 0));

            al_flip_display();
        }
    }

    al_destroy_display(tela);
    al_destroy_bitmap(canga.sprite);
    al_destroy_bitmap(sprite_formiga);
    al_destroy_bitmap(sprite_tatu);
    al_destroy_timer(tick_timer);
    al_destroy_event_queue(fila);
    al_destroy_bitmap(balas->sprite);
    al_destroy_bitmap(cuspe);

    free(balas);
    free(formiga_bala);
    free(formigas);
    free(homem_tatus);

    return 0;
}