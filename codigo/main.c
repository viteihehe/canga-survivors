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

int mapa01_blocos[MAPA_LINHAS][MAPA_COLUNAS] = {
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

enum EDecoracao {
    DN, // Nada
    DG, // Grama
    DP, // Pedrinhas
};

int mapa01_decos[MAPA_LINHAS][MAPA_COLUNAS] = {
    {},
    {},
    {DN, DN, DN, DN, DN, DN, DN, DN, DN, DN, DN, DN, DN, DG, DG},
    {DN, DN, DN, DP, DN, DN, DN, DN, DN, DN, DN, DN, DN, DN, DG},
    {DN, DN, DN, DP},
    {DN, DN, DN, DN, DP},
    {DN, DN, DN, DN, DN, DN, DN, DN, DN, DG},
    {DN, DN, DN, DN, DN, DN, DN, DN, DG, DG, DG},
    {DN, DN, DN, DN, DN, DN, DN, DG, DG, DG, DG},
    {DN, DN, DN, DN, DN, DN, DN, DN, DG, DG},
    {DN, DN, DN, DN, DN, DN, DN, DN, DN, DG},
    {},
    {DN, DN, DN, DN, DN, DN, DN, DN, DN, DN, DN, DN, DN, DN, DN, DN, DN},
    {DN, DG, DN, DN, DN, DN, DN, DN, DN, DN, DN, DN, DN, DN, DN, DP, DP},
    {DN, DG, DG, DN, DN, DN, DN, DN, DN, DN, DN, DN, DN, DN, DN, DN, DP, DP},
    {},
};

typedef struct {
    ALLEGRO_BITMAP *canga;

    ALLEGRO_BITMAP *tatu;
    ALLEGRO_BITMAP *formiga;
    ALLEGRO_BITMAP *cuspe;

    ALLEGRO_BITMAP *areia;
    ALLEGRO_BITMAP *cacto;
    ALLEGRO_BITMAP *pedra;
    ALLEGRO_BITMAP *arbusto;

    ALLEGRO_BITMAP *sombra;
    ALLEGRO_BITMAP *bala;
    ALLEGRO_BITMAP *grama;
    ALLEGRO_BITMAP *pedrinhas;

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

            // ----------
            // Blocos
            // ----------
            switch (mapa01_blocos[lin][col]) {
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
            }

            // ----------
            // Decorações
            // ----------
            switch (mapa01_decos[lin][col]) {
            case DG:
                al_draw_scaled_bitmap(sprites.grama, 0, 0, 16, 16, x, y, 48, 48,
                                      0);
                break;

            case DP:
                al_draw_scaled_bitmap(sprites.pedrinhas, 0, 0, 16, 16, x, y, 48,
                                      48, 0);
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
    if (mapa01_blocos[cel_y][cel_x] >= 1) {
        return 1;
    }

    // Superior Direito
    cel_x = (x + tam_box) / TAM_BLOCOS;
    cel_y = (y - tam_box) / TAM_BLOCOS;
    // al_draw_filled_circle(x + tam_box, y - tam_box, 3, al_map_rgb(0, 255,
    // 0));
    if (mapa01_blocos[cel_y][cel_x] >= 1) {
        return 1;
    }

    // Inferior Esquerdo
    cel_x = (x - tam_box) / TAM_BLOCOS;
    cel_y = (y + tam_box) / TAM_BLOCOS;
    // al_draw_filled_circle(x - tam_box, y + tam_box, 3, al_map_rgb(0, 255,
    // 0));
    if (mapa01_blocos[cel_y][cel_x] >= 1) {
        return 1;
    }

    // Inferior Direito
    cel_x = (x + tam_box) / TAM_BLOCOS;
    cel_y = (y + tam_box) / TAM_BLOCOS;
    // al_draw_filled_circle(x + tam_box, y + tam_box, 3, al_map_rgb(0, 255,
    // 0));
    if (mapa01_blocos[cel_y][cel_x] >= 1) {
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

    int cooldown_arma;
    long tempo_ultimo_disparo;

    int vida;
    int dano_delay;
    bool vivo;
    int ultimo_dano;
} Jogador;

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
    Bala *balas;
    int quantidade_de_ataques;
    int tamanho_sprite;
} Inimigo;

int colide_no_cenario(int x, int y, int tam_box);
void logicaBalaFormiga(Inimigo *inimigo);
void colisaoInimigos(Inimigo inimigos[], int *indice, int tamanho,
                     int tamanhosprite);
void reajusteInimigos(Inimigo inimigos[], int *indice);
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
    int temp = -((jogador->vida * 15) - 5) / 2;
    for (int x = 0; x < jogador->vida; x++) {
        al_draw_filled_circle(jogador->x + temp, jogador->y - 40, 5,
                              al_map_rgb(255, 0, 0));
        temp += 15;
    }
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
    if (al_get_timer_count(tick_timer) < jogador->tempo_ultimo_disparo) {
        return;
    }

    Bala bala_temp = {sprites.bala,  jogador->x, jogador->y,
                      jogador->mira, true,       1};

    (*dest_quant)++;
    *balas = realloc(*balas, sizeof(Bala) * *dest_quant);
    (*balas)[*dest_quant - 1] = bala_temp;

    jogador->tempo_ultimo_disparo =
        al_get_timer_count(tick_timer) + jogador->cooldown_arma;
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
                  double *ultimo_spawn_tatu, double *ultimo_spawn_formiga,
                  int *indice_tatu, int *indice_formiga) {
    double cooldoown_tatu = 2;
    double cooldoown_formiga = 4;

    if (*counts - *ultimo_spawn_tatu >= cooldoown_tatu && *indice_tatu < 50) {

        Inimigo tatu_temp = {TATU, 28,   0, 0, 0,    sprite_tatu, 3, 1,
                             1,    true, 2, 0, NULL, 0,           64};

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

        Inimigo temp_formiga = {FORMIGA, 22, 0,   0,    0, sprite_formiga,
                                2,       1,  0.5, true, 2, 0,
                                NULL,    0,  48};
        temp_formiga.balas = NULL;
        temp_formiga.quantidade_de_ataques = 0;

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
            formigas[*indice_formiga].posy = ALTURA - 120;
            break;

        default:
            break;
        }

        *ultimo_spawn_formiga = *counts;
        (*indice_formiga)++;
    }
}

void inimigosLogica(Inimigo inimigos[], int *indice, Jogador canga,
                    double *counts, ALLEGRO_BITMAP *cuspe) {
    const int disparo_cooldown = 2;
    int colisao = 0;
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
            if (!colide_no_cenario((int)x_futuro, (int)inimigos[i].posy, 64)) {
                inimigos[i].posx = x_futuro;
            }
            if (!colide_no_cenario((int)inimigos[i].posx, (int)y_futuro, 64)) {
                inimigos[i].posy = y_futuro;
            }
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

                    Bala tempBala = {cuspe,
                                     (int)(inimigos[i].posx),
                                     (int)(inimigos[i].posy),
                                     {false, false, false, false},
                                     true,
                                     1};

                    inimigos[i].balas = realloc(
                        inimigos[i].balas,
                        sizeof(Bala) * (inimigos[i].quantidade_de_ataques + 1));

                    inimigos[i].balas[inimigos[i].quantidade_de_ataques] =
                        tempBala;

                    Bala *bala_formiga =
                        &inimigos[i].balas[inimigos[i].quantidade_de_ataques];

                    if (canga.x < inimigos[i].posx - 48) {
                        bala_formiga->direcoes.esq = true;
                    } else if (canga.x > inimigos[i].posx + 48) {
                        bala_formiga->direcoes.dir = true;
                    } else if (canga.y > inimigos[i].posy - 48) {
                        bala_formiga->direcoes.baixo = true;
                    } else if (canga.y < inimigos[i].posy + 48) {
                        bala_formiga->direcoes.cima = true;
                    }
                    inimigos[i].ultimo_ataque = *counts;
                    inimigos[i].quantidade_de_ataques++;
                }
                logicaBalaFormiga(&inimigos[i]);
            }
        }
    }

    colisaoInimigos(inimigos, indice, colisao, inimigos->tamanho_sprite);
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
            int colisao_x = tamanho;
            int colisao_y = tamanho;
            if (abs(inimigos[i].posx - inimigos[j].posx) <= colisao_x &&
                abs(inimigos[i].posy - inimigos[j].posy) <= colisao_y) {
                if (inimigos[i].posx < inimigos[j].posx) {
                    inimigos[i].posx -= inimigos[i].velocidade / 2;
                    inimigos[j].posx += inimigos[j].velocidade / 2;
                } else {
                    inimigos[i].posx += inimigos[i].velocidade / 2;
                    inimigos[j].posx -= inimigos[j].velocidade / 2;
                }
                if (inimigos[i].posy < inimigos[j].posy) {
                    inimigos[i].posy -= inimigos[i].velocidade / 2;
                    inimigos[j].posy += inimigos[j].velocidade / 2;
                } else {
                    inimigos[i].posy += inimigos[i].velocidade / 2;
                    inimigos[j].posy -= inimigos[j].velocidade / 2;
                }
            }
        }
    }
}

void colisaoBala(Bala *bala_atual, Inimigo *inimigo_atual, int colisao) {
    if (bala_atual->ativa && inimigo_atual->ativo) {
        if (abs(bala_atual->x - inimigo_atual->posx) < colisao &&
            abs(bala_atual->y - inimigo_atual->posy) < colisao) {
            inimigo_atual->vida -= bala_atual->dano;
            bala_atual->ativa = false;
        }
    }
}

void processamentoBala(Inimigo inimigos[], int *indice, Bala balas[],
                       int *max_balas, int colisao) {
    for (int i = 0; i < *indice; i++) {
        if (!inimigos[i].ativo)
            continue;
        for (int j = 0; j < *max_balas; j++) {
            if (!balas[j].ativa)
                continue;

            colisaoBala(&balas[j], &inimigos[i], colisao);
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

void reajusteInimigos(Inimigo inimigos[], int *indice) {
    int vivos = 0;
    for (int i = 0; i < *indice; i++) {
        if (inimigos[i].ativo) {
            inimigos[vivos++] = inimigos[i];
        } else {
            free(inimigos[i].balas);
        }
    }
    *indice = vivos;
}

void desenharInimigo(Inimigo inimigos[], int indice, int *contador_frames,
                     Jogador canga) {
    for (int i = 0; i < indice; i++) {
        if (inimigos[i].ativo) {
            frames(contador_frames, &inimigos[i]);
            int png_x = inimigos[i].frame_atual * inimigos[i].tamanho_sprite;
            int png_y = 0;
            int flip = 0;
            if (inimigos[i].posx < canga.x) {
                flip = ALLEGRO_FLIP_HORIZONTAL;
            }

            al_draw_bitmap_region(
                inimigos[i].sprite, png_x, png_y, inimigos[i].tamanho_sprite,
                inimigos[i].tamanho_sprite,
                inimigos[i].posx - inimigos[i].tamanho_sprite / 2.0,
                inimigos[i].posy - inimigos[i].tamanho_sprite / 2.0, flip);

            int temp = -((inimigos[i].vida * 15) - 5) / 2;
            for (int x = 0; x < inimigos[i].vida; x++) {
                al_draw_filled_circle(inimigos[i].posx + temp,
                                      inimigos[i].posy - 40, 5,
                                      al_map_rgb(255, 0, 0));
                temp += 15;
            }
            if (inimigos[i].balas != NULL) {
                Bala *formiga_bala = inimigos[i].balas;
                if (inimigos[i].comportamento == FORMIGA) {
                    for (int j = 0; j < inimigos[i].quantidade_de_ataques;
                         j++) {
                        if (formiga_bala[j].ativa) {
                            al_draw_bitmap(formiga_bala[j].sprite,
                                           formiga_bala[j].x - 8,
                                           formiga_bala[j].y - 8, 0);
                        }
                    }
                }
            }
        }
    }
}

void logicaBalaFormiga(Inimigo *inimigo) {
    int velocidade_bala = 2;
    for (int i = 0; i < inimigo->quantidade_de_ataques; i++) {
        if (inimigo->balas[i].ativa) {
            if (inimigo->balas[i].direcoes.cima) {
                inimigo->balas[i].y -= velocidade_bala;
            } else if (inimigo->balas[i].direcoes.baixo) {
                inimigo->balas[i].y += velocidade_bala;
            }
            if (inimigo->balas[i].direcoes.dir) {
                inimigo->balas[i].x += velocidade_bala;
            } else if (inimigo->balas[i].direcoes.esq) {
                inimigo->balas[i].x -= velocidade_bala;
            }
        }
        if (colide_no_cenario(inimigo->balas[i].x, inimigo->balas[i].y, 12)) {
            inimigo->balas[i].ativa = 0;
        }
    }
}

void danoJogador(Inimigo inimigos[], Jogador *canga, int indice,
                 double counts) {
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
            }
        }
        if (inimigos[i].comportamento == FORMIGA) {
            for (int j = 0; j < inimigos[i].quantidade_de_ataques; j++) {
                if (!inimigos[i].balas[j].ativa) {
                    continue;
                }

                if (abs(inimigos[i].balas[j].x - canga->x) < 40 &&
                    abs(inimigos[i].balas[j].y - canga->y) < 40 &&
                    counts - canga->ultimo_dano >= canga->dano_delay) {
                    canga->vida -= 1;
                    canga->ultimo_dano = counts;
                    inimigos[i].balas[j].ativa = false;
                }
            }
        }
    }

    if (canga->vida <= 0)
        canga->vivo = false;
}

typedef struct {
    FolhaSprites sprites;
    Jogador canga;
    Bala *balas;
    int quant_balas;
    int indice_tatu;
    int indice_formiga;
    Inimigo *homem_tatus;
    Inimigo *formigas;
    double ultimo_spawn_formiga;
    double ultimo_spawn_tatu;
    double counts;
} EstadoGlobal;

/*
    Gera um novo estado de jogo. A função pede a folha de sprites para evitar
   carregar eles de novo todo restart.
*/
EstadoGlobal gerar_estado(FolhaSprites sprites) {
    Jogador canga = {
        .sprite = sprites.canga,

        .x = LARGURA / 2,
        .y = ALTURA / 2,

        .vida = 3,
        .vivo = true,
        .cooldown_arma = 30,
        .dano_delay = 1,
    };

    EstadoGlobal globs = {
        .sprites = sprites,
        .canga = canga,

        /*
            Mesmo que todos esses já virem NULL por padrão, vou deixar eles
           documentados aqui pra ajudar a lembrar na hora de dar o free() no
           reiniciar_estado().
        */
        .balas = NULL,
        .homem_tatus = malloc(50 * sizeof(Inimigo)),
        .formigas = malloc(50 * sizeof(Inimigo)),
    };

    return globs;
}

/*
    Reinicia o estado de um jogo anterior.
*/
void reiniciar_estado(EstadoGlobal *antigo) {
    free(antigo->balas);
    free(antigo->homem_tatus);
    free(antigo->formigas);

    *antigo = gerar_estado(antigo->sprites);
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
    ALLEGRO_FONT *fonte =
        al_load_ttf_font("./materiais/fontes/FiftiesMovies.ttf", 32, 0);

    ALLEGRO_TIMER *tick_timer = al_create_timer(1.0 / FPS);
    al_register_event_source(fila, al_get_timer_event_source(tick_timer));
    al_start_timer(tick_timer);

    // ----------
    // Sprites
    // ----------
    FolhaSprites sprites = {
        .canga = al_load_bitmap("./materiais/sprites/canga.png"),
        .tatu = al_load_bitmap("./materiais/sprites/peba2_1.png"),
        .formiga = al_load_bitmap("./materiais/sprites/formiga2.png"),
        .cuspe = al_load_bitmap("./materiais/sprites/cuspe.png"),

        .bala = al_load_bitmap("./materiais/sprites/bala.png"),
        .sombra = al_load_bitmap("./materiais/sprites/sombra.png"),

        .areia = al_load_bitmap("./materiais/sprites/mapa/areia.png"),
        .cacto = al_load_bitmap("./materiais/sprites/mapa/cacto.png"),
        .pedra = al_load_bitmap("./materiais/sprites/mapa/pedra.png"),
        .arbusto = al_load_bitmap("./materiais/sprites/mapa/arbusto.png"),

        .grama = al_load_bitmap("./materiais/sprites/mapa/grama.png"),
        .pedrinhas = al_load_bitmap("./materiais/sprites/mapa/pedrinhas.png"),
    };

    // ----------
    // Globais
    // ----------
    /*
        Eu criei essa struct só pra facilitar na hora do restart do jogo.
       Por mais que seja tentador, evite ao MÁÁÁXIMO passar esse cara como
       argumento de uma função. Vai evitar macarronada no código. Obrigado!
    */
    EstadoGlobal globs = gerar_estado(sprites);

    // ----------
    // Janela
    // ----------
    al_set_display_icon(tela, globs.sprites.cacto);
    al_set_window_title(tela, "Lampião Survivors");

    // ---------
    // Inimigos
    // ---------
    int contador_frames = 0;

    // ----------
    // Loop Principal
    // ----------
    ALLEGRO_EVENT evento;
    for (;;) {
        al_wait_for_event(fila, &evento);

        capturar_movimento(evento, &globs.canga.movimento);
        capturar_mira(evento, &globs.canga.mira);

        if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            break;
        }

        if (!globs.canga.vivo && evento.type == ALLEGRO_EVENT_KEY_DOWN &&
            evento.keyboard.keycode == ALLEGRO_KEY_SPACE) {
            reiniciar_estado(&globs);
            continue;
        }

        if (evento.type == ALLEGRO_EVENT_TIMER) {
            if (globs.canga.vivo) {
                criar_bala_jogador(&globs.balas, &globs.quant_balas,
                                   &globs.canga, tick_timer, globs.sprites);

                //--------
                // Inimigos
                //--------
                globs.counts = al_get_time();
                criarInimigo(globs.homem_tatus, globs.formigas, &globs.counts,
                             globs.sprites.formiga, globs.sprites.tatu,
                             &globs.ultimo_spawn_tatu,
                             &globs.ultimo_spawn_formiga, &globs.indice_tatu,
                             &globs.indice_formiga);

                inimigosLogica(globs.homem_tatus, &globs.indice_tatu,
                               globs.canga, &globs.counts, globs.sprites.cuspe);
                inimigosLogica(globs.formigas, &globs.indice_formiga,
                               globs.canga, &globs.counts, globs.sprites.cuspe);
                processamentoBala(globs.homem_tatus, &globs.indice_tatu,
                                  globs.balas, &globs.quant_balas, 28);
                processamentoBala(globs.formigas, &globs.indice_formiga,
                                  globs.balas, &globs.quant_balas, 22);
                danoJogador(globs.homem_tatus, &globs.canga, globs.indice_tatu,
                            globs.counts);
                danoJogador(globs.formigas, &globs.canga, globs.indice_formiga,
                            globs.counts);
                // ----------
                // Frames
                // ----------
                // al_draw_bitmap(cenario, 0, 0, ALLEGRO_FLIP_HORIZONTAL);
                al_draw_filled_rectangle(0, 0, LARGURA, ALTURA,
                                         al_map_rgb(0, 0, 0));
                redesenhar_mapa(globs.sprites);

                mover_jogador(globs.canga.movimento, &globs.canga);
                desenharInimigo(globs.homem_tatus, globs.indice_tatu,
                                &contador_frames, globs.canga);
                desenharInimigo(globs.formigas, globs.indice_formiga,
                                &contador_frames, globs.canga);
                mover_balas(globs.balas, globs.quant_balas);
                // al_draw_filled_circle(canga.x, canga.y, 5, al_map_rgb(255, 0,
                // 0));

            } else {
                // Temporário
                redesenhar_mapa(sprites);
                al_draw_filled_rectangle(0, (ALTURA / 2.0) - 80, LARGURA,
                                         (ALTURA / 2.0) + 80,
                                         al_map_rgba(0, 0, 0, 240));

                al_draw_text(fonte, al_map_rgb(255, 255, 255), LARGURA / 6.0,
                             (ALTURA / 2.0) - 30, 0, "SE LASCOU!");

                al_draw_text(fonte, al_map_rgb(150, 150, 150), LARGURA / 6.0,
                             (ALTURA / 2.0) + 10, 0,
                             "Pressione [ESPAÇO] para reiniciar.");
            }
            al_flip_display();
        }
    }

    al_destroy_display(tela);
    // al_destroy_bitmap(canga.sprite);
    // al_destroy_bitmap(sprite_formiga);
    // al_destroy_bitmap(sprite_tatu);
    al_destroy_timer(tick_timer);
    al_destroy_event_queue(fila);
    // al_destroy_bitmap(balas->sprite);
    // al_destroy_bitmap(cuspe);
    al_destroy_font(fonte);
    // free(formigas);
    // free(homem_tatus);

    return 0;
}