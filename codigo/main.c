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
} Bala;

typedef struct {
    int posx;
    int posy;
    ALLEGRO_BITMAP *monstro;
    int vida;
    int dano;
    int velocidade;
    int vivo;
} Homem_tatu;

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
    bool ativa;
} BalaInimigo;

typedef struct {
    int posx;
    int posy;
    ALLEGRO_BITMAP *sprite;
    int vida;
    int dano;
    float velocidade;
    int vivo;
    int ultimo_disparo;
} Formiga;

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

void frames(int *contador_frames, int frame_delay, int *frame_atual_tatu,
            int *frame_atual_formiga, int total_frames_tatu,
            int total_frames_formiga) {
    (*contador_frames)++;

    if (*contador_frames >= frame_delay) {
        (*frame_atual_tatu)++;
        (*frame_atual_formiga)++;
        if (*frame_atual_tatu >= total_frames_tatu) {
            *frame_atual_tatu = 0;
        }
        if (*frame_atual_formiga >= total_frames_formiga) {
            *frame_atual_formiga = 0;
        }
        *contador_frames = 0;
    }
}

// Lógica do inimigo tatu
void criarTatu(Homem_tatu homem_tatus[], double *counts, float *ultimo_spawn,
               int cooldown, int *indice_tatu, ALLEGRO_BITMAP *sprite_tatu) {

    if (*counts - *ultimo_spawn >= cooldown && *indice_tatu < 50) {
        homem_tatus[*indice_tatu].monstro = sprite_tatu;
        homem_tatus[*indice_tatu].velocidade = 1;
        homem_tatus[*indice_tatu].vida = 3;
        homem_tatus[*indice_tatu].dano = 1;
        homem_tatus[*indice_tatu].vivo = 1;

        int spawn = rand() % 3;

        switch (spawn) {
        case 0:
            homem_tatus[*indice_tatu].posx = 80;
            homem_tatus[*indice_tatu].posy = ALTURA / 2;
            break;
        case 1:
            homem_tatus[*indice_tatu].posx = LARGURA - 80;
            homem_tatus[*indice_tatu].posy = ALTURA / 2;
            break;
        case 2:
            homem_tatus[*indice_tatu].posx = LARGURA / 2;
            homem_tatus[*indice_tatu].posy = 50;
            break;
        case 3:
            homem_tatus[*indice_tatu].posx = LARGURA / 2;
            homem_tatus[*indice_tatu].posy = 680;
            break;

        default:
            break;
        }

        *ultimo_spawn = *counts;
        (*indice_tatu)++;
    }
}

void logicaTatu(Homem_tatu homem_tatus[], int *indice_tatu, Jogador canga) {
    for (int i = 0; i < *indice_tatu; i++) {
        if (homem_tatus[i].posx < canga.x) {
            homem_tatus[i].posx += homem_tatus[i].velocidade;
        }
        if (homem_tatus[i].posy < canga.y) {
            homem_tatus[i].posy += homem_tatus[i].velocidade;
        }
        if (homem_tatus[i].posx > canga.x) {
            homem_tatus[i].posx -= homem_tatus[i].velocidade;
        }
        if (homem_tatus[i].posy > canga.y) {
            homem_tatus[i].posy -= homem_tatus[i].velocidade;
        }
    }
}

void colisaoTatu(Homem_tatu homem_tatus[], int *indice_tatu) {
    for (int i = 0; i < *indice_tatu; i++) {
        if (!homem_tatus[i].vivo)
            continue;
        for (int j = 0; j < *indice_tatu; j++) {
            if (!homem_tatus[i].vivo)
                continue;
            int colisao_x = 64;
            int colisao_y = 64;
            if (abs(homem_tatus[i].posx - homem_tatus[j].posx) <= colisao_x &&
                abs(homem_tatus[i].posy - homem_tatus[j].posy) <= colisao_y) {
                if (homem_tatus[i].posx < homem_tatus[j].posx) {
                    homem_tatus[i].posx -= homem_tatus[i].velocidade;
                    homem_tatus[j].posx += homem_tatus[j].velocidade;
                } else {
                    homem_tatus[i].posx += homem_tatus[i].velocidade;
                    homem_tatus[j].posx -= homem_tatus[j].velocidade;
                }
                if (homem_tatus[i].posy < homem_tatus[j].posx) {
                    homem_tatus[i].posy -= homem_tatus[i].velocidade;
                    homem_tatus[j].posy += homem_tatus[j].velocidade;
                } else {
                    homem_tatus[i].posy += homem_tatus[i].velocidade;
                    homem_tatus[j].posy -= homem_tatus[j].velocidade;
                }
            }
        }
    }
}

void desenhoTatu(Homem_tatu homem_tatus[], int frame_atual_tatu,
                 int indice_tatu, Jogador canga) {
    for (int i = 0; i < indice_tatu; i++) {
        if (homem_tatus[i].vivo) {
            int png_x = frame_atual_tatu * 64;
            int png_y = 0;
            int flip = 0;

            if (homem_tatus[i].posx < canga.x) {
                flip = ALLEGRO_FLIP_HORIZONTAL;
            }

            al_draw_bitmap_region(homem_tatus[i].monstro, png_x, png_y, 64, 64,
                                  homem_tatus[i].posx, homem_tatus[i].posy,
                                  flip);
        }
    }
}

// Lógica do inimigo formiga
void criacaoFormiga(Formiga formigas[], double *counts,
                    ALLEGRO_BITMAP *sprite_formiga, float *ultimo_spawn_formiga,
                    float cooldown_formiga, int *indice_formiga) {

    if (*counts - *ultimo_spawn_formiga >= cooldown_formiga &&
        *indice_formiga < 50) {
        formigas[*indice_formiga].dano = 1;
        formigas[*indice_formiga].vida = 2;
        formigas[*indice_formiga].velocidade = 0.5;
        formigas[*indice_formiga].vivo = 1;
        formigas[*indice_formiga].sprite = sprite_formiga;
        formigas[*indice_formiga].ultimo_disparo = 0;

        int spawn = rand() % 3;

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
            formigas[*indice_formiga].posy = 50;
            break;
        case 3:
            formigas[*indice_formiga].posx = LARGURA / 2;
            formigas[*indice_formiga].posy = 680;
            break;

        default:
            break;
        }
        *ultimo_spawn_formiga = (float)*counts;
        (*indice_formiga)++;
    }
}

void logicaFormiga(Formiga formigas[], BalaInimigo formiga_bala[],
                   int *indice_formiga, Jogador canga, double *counts,
                   int disparo_cooldown, int *cont_disparo) {
    // logica formiga
    for (int i = 0; i < *indice_formiga; i++) {
        if (formigas[i].vivo) {
            if (formigas[i].posx > 10 && formigas[i].posx < 1270) {
                if (formigas[i].posx >= canga.x - 150) {
                    formigas[i].posx -= formigas[i].velocidade;
                } else {
                    formigas[i].posx += formigas[i].velocidade;
                }
                if (formigas[i].posx <= canga.x + 150) {
                    formigas[i].posx += formigas[i].velocidade;
                } else {
                    formigas[i].posx -= formigas[i].velocidade;
                }
                if (formigas[i].posy >= canga.y - 100) {
                    formigas[i].posy -= formigas[i].velocidade;
                } else {
                    formigas[i].posy += formigas[i].velocidade;
                }
                if (formigas[i].posy <= canga.y + 100) {
                    formigas[i].posy += formigas[i].velocidade;
                } else {
                    formigas[i].posy -= formigas[i].velocidade;
                }
            }
            if (*counts - formigas[i].ultimo_disparo >= disparo_cooldown &&
                *cont_disparo < 2000) {
                BalaInimigo *bala_formiga = &formiga_bala[*cont_disparo];
                bala_formiga->ativa = true;

                if (canga.x < formigas[i].posx - 48) {
                    bala_formiga->movimento = ESQUERDA;
                } else if (canga.x > formigas[i].posx + 48) {
                    bala_formiga->movimento = DIREITA;
                } else if (canga.y > formigas[i].posy - 48) {
                    bala_formiga->movimento = BAIXO;
                } else if (canga.y < formigas[i].posy - 48) {
                    bala_formiga->movimento = CIMA;
                }

                bala_formiga->posx = formigas[i].posx + 30;
                bala_formiga->posy = formigas[i].posy + 20;
                formigas[i].ultimo_disparo = *counts;
                (*cont_disparo)++;
            }
        }
    }
}

void logicaBalaFormiga(BalaInimigo formiga_bala[], int *cont_disparo) {
    int velocidade_bala = 3;
    for (int i = 0; i < *cont_disparo; i++) {
        if (formiga_bala[i].ativa) {
            if (formiga_bala[i].movimento == CIMA) {
                formiga_bala[i].posy -= velocidade_bala;
            } else if (formiga_bala[i].movimento == BAIXO) {
                formiga_bala[i].posy += velocidade_bala;
            } else if (formiga_bala[i].movimento == DIREITA) {
                formiga_bala[i].posx += velocidade_bala;
            } else if (formiga_bala[i].movimento == ESQUERDA) {
                formiga_bala[i].posx -= velocidade_bala;
            }

            if (formiga_bala[i].posx < 0 || formiga_bala[i].posx > LARGURA ||
                formiga_bala[i].posy < 0 || formiga_bala[i].posy > ALTURA) {
                formiga_bala[i].ativa = false;
            }
        }
    }
}

void colisaoFormiga(Formiga formigas[], int *indice_formiga) {
    for (int i = 0; i < *indice_formiga; i++) {
        if (!formigas[i].vivo)
            continue;
        for (int j = 0; j < *indice_formiga; j++) {
            if (!formigas[j].vivo)
                continue;
            int colisao_x = 48;
            int colisao_y = 48;
            if (abs(formigas[i].posx - formigas[j].posx) <= colisao_x &&
                abs(formigas[i].posy - formigas[j].posy) <= colisao_y) {
                if (formigas[i].posx < formigas[j].posx) {
                    formigas[i].posx -= formigas[i].velocidade;
                    formigas[j].posx += formigas[j].velocidade;
                } else {
                    formigas[i].posx += formigas[i].velocidade;
                    formigas[j].posx -= formigas[j].velocidade;
                }
                if (formigas[i].posy < formigas[j].posx) {
                    formigas[i].posy -= formigas[i].velocidade;
                    formigas[j].posy += formigas[j].velocidade;
                } else {
                    formigas[i].posy += formigas[i].velocidade;
                    formigas[j].posy -= formigas[j].velocidade;
                }
            }
        }
    }
}

void desenhoFormiga(Formiga formigas[], int indice_formiga,
                    int frame_atual_formiga, Jogador canga,
                    BalaInimigo formiga_bala[], int cont_disparo) {
    for (int i = 0; i < indice_formiga; i++) {
        if (formigas[i].vivo) {
            int png_x = frame_atual_formiga * 48;
            int png_y = 0;
            int flip = 0;

            if (formigas[i].posx > canga.x) {
                flip = ALLEGRO_FLIP_HORIZONTAL;
            }

            al_draw_bitmap_region(formigas[i].sprite, png_x, png_y, 48, 48,
                                  formigas[i].posx, formigas[i].posy, flip);
        }
    }
    for (int i = 0; i < cont_disparo; i++) {
        if (formiga_bala[i].ativa) {
            al_draw_filled_circle(formiga_bala[i].posx, formiga_bala[i].posy, 5,
                                  al_map_rgb(0, 0, 0));
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

    // Homem Tatu
    Homem_tatu *homem_tatus = (Homem_tatu *)malloc(50 * sizeof(Homem_tatu));
    ALLEGRO_BITMAP *sprite_tatu =
        al_load_bitmap("./materiais/sprites/peba2_1.png");

    int indice_tatu = 0;
    float ultimo_spawn_tatu = 0;
    const float cooldown_tatu = 2;

    // Formiga
    BalaInimigo *formiga_bala =
        (BalaInimigo *)malloc(2000 * sizeof(BalaInimigo));
    Formiga *formigas = (Formiga *)malloc(50 * sizeof(Formiga));
    ALLEGRO_BITMAP *sprite_formiga =
        al_load_bitmap("./materiais/sprites/formiga2.png");

    int indice_formiga = 0;
    float ultimo_spawn_formiga = 0;
    const float cooldown_formiga = 5;
    const float disparo_cooldown = 2;
    int cont_disparo = 0;

    //---------
    // Animações
    //---------
    int contador_frames = 0;
    const int frame_delay = 10;
    int frame_atual_formiga = 0;
    const int total_frames_formiga = 2;
    const int total_frames_tatu = 2;
    int frame_atual_tatu = 0;

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
            double tempo_timer = al_get_timer_count(tick_timer);
            double tempo_em_segundos = tempo_timer / 60;
            frames(&contador_frames, frame_delay, &frame_atual_tatu,
                   &frame_atual_formiga, total_frames_tatu,
                   total_frames_formiga);

            // Homem Tatu
            /*
            criarTatu(homem_tatus, &tempo_em_segundos, &ultimo_spawn_tatu,
                      cooldown_tatu, &indice_tatu, sprite_tatu);
            logicaTatu(homem_tatus, &indice_tatu, canga);
            colisaoTatu(homem_tatus, &indice_tatu);*/

            // Formiga
            /*
            criacaoFormiga(formigas, &tempo_em_segundos, sprite_formiga,
                           &ultimo_spawn_formiga, cooldown_formiga,
                           &indice_formiga);
            logicaFormiga(formigas, formiga_bala, &indice_formiga, canga,
                          &tempo_em_segundos, disparo_cooldown, &cont_disparo);
            logicaBalaFormiga(formiga_bala, &cont_disparo);
            colisaoFormiga(formigas, &indice_formiga);*/

            // ----------
            // Frames
            // ----------
            // al_draw_bitmap(cenario, 0, 0, ALLEGRO_FLIP_HORIZONTAL);
            al_draw_filled_rectangle(0, 0, LARGURA, ALTURA,
                                     al_map_rgb(0, 0, 0));
            redesenhar_mapa(sprites);

            mover_jogador(canga.movimento, &canga);
            desenhoTatu(homem_tatus, frame_atual_tatu, indice_tatu, canga);
            desenhoFormiga(formigas, indice_formiga, frame_atual_formiga, canga,
                           formiga_bala, cont_disparo);

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

    free(formiga_bala);
    free(formigas);
    free(homem_tatus);

    return 0;
}