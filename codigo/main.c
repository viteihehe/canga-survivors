#include "cenario.h"
#include "consts.h"
#include "jogador.h"
#include "powerups.h"
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/altime.h>
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

typedef enum { CIMA, BAIXO, DIREITA, ESQUERDA } Direcoes;

typedef enum {
    TATU,
    FORMIGA,
} Ecomportamento;

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

void logicaBalaFormiga(Inimigo *inimigo);
void colisaoInimigos(Inimigo inimigos[], int *indice, int tamanho,
                     int tamanhosprite);
void reajusteInimigos(Inimigo inimigos[], int *indice);

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

void criarInimigo(Inimigo **tatus, Inimigo **formigas, double *counts,
                  ALLEGRO_BITMAP *sprite_formiga, ALLEGRO_BITMAP *sprite_tatu,
                  double *ultimo_spawn_tatu, double *ultimo_spawn_formiga,
                  int *indice_tatu, int *indice_formiga, double *cooldoown_tatu,
                  double *cooldoown_formiga) {

    /*
        Função de criar todos os inimigos de maneira compacta
    */

    if (*counts - *ultimo_spawn_tatu >= *cooldoown_tatu && *indice_tatu < 50) {

        Inimigo tatu_temp = {TATU, 28,   0, 0, 0,    sprite_tatu, 3, 1,
                             1,    true, 2, 0, NULL, 0,           64};

        (*indice_tatu)++;
        *tatus = realloc(*tatus, sizeof(Inimigo) * (*indice_tatu + 1));
        (*tatus)[*indice_tatu - 1] = tatu_temp;

        int spawn = rand() % 4;

        switch (spawn) {
        case 0:
            (*tatus)[*indice_tatu - 1].posx = 80;
            (*tatus)[*indice_tatu - 1].posy = ALTURA / 2;
            break;

        case 1:
            (*tatus)[*indice_tatu - 1].posx = LARGURA - 200;
            (*tatus)[*indice_tatu - 1].posy = ALTURA / 2;
            break;

        case 2:
            (*tatus)[*indice_tatu - 1].posx = LARGURA / 2;
            (*tatus)[*indice_tatu - 1].posy = ALTURA - 660;
            break;

        case 3:
            (*tatus)[*indice_tatu - 1].posx = LARGURA / 2;
            (*tatus)[*indice_tatu - 1].posy = ALTURA - 160;
            break;

        default:
            break;
        }

        *ultimo_spawn_tatu = *counts;
    }
    if (*counts - *ultimo_spawn_formiga >= *cooldoown_formiga &&
        *indice_formiga < 100) {

        Inimigo temp_formiga = {FORMIGA, 22, 0,   0,    0, sprite_formiga,
                                2,       1,  0.5, true, 2, 0,
                                NULL,    0,  48};
        temp_formiga.balas = NULL;
        temp_formiga.quantidade_de_ataques = 0;

        (*indice_formiga)++;
        *formigas = realloc(*formigas, sizeof(Inimigo) * (*indice_formiga));
        (*formigas)[*indice_formiga - 1] = temp_formiga;

        int spawn = rand() % 4;

        switch (spawn) {
        case 0:
            (*formigas)[*indice_formiga - 1].posx = 80;
            (*formigas)[*indice_formiga - 1].posy = ALTURA / 2;
            break;
        case 1:
            (*formigas)[*indice_formiga - 1].posx = LARGURA - 200;
            (*formigas)[*indice_formiga - 1].posy = ALTURA / 2;
            break;
        case 2:
            (*formigas)[*indice_formiga - 1].posx = LARGURA / 2;
            (*formigas)[*indice_formiga - 1].posy = ALTURA - 660;
            break;
        case 3:
            (*formigas)[*indice_formiga - 1].posx = LARGURA / 2;
            (*formigas)[*indice_formiga - 1].posy = ALTURA - 160;
            break;

        default:
            break;
        }

        *ultimo_spawn_formiga = *counts;
    }
}

void inimigosLogica(Inimigo inimigos[], int *indice, Jogador canga,
                    double *counts, ALLEGRO_BITMAP *cuspe) {
    const int disparo_cooldown = 2;
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
            if (!colide_no_cenario((int)x_futuro, (int)inimigos[i].posy, 64)) {
                inimigos[i].posx = x_futuro;
            }
            if (!colide_no_cenario((int)inimigos[i].posx, (int)y_futuro, 64)) {
                inimigos[i].posy = y_futuro;
            }
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

    // Isso tá conseguindo chegar como NULL aqui. Não deveria
    if (inimigos == NULL) {
        return;
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

void colisaoBala(Bala *bala_atual, Inimigo *inimigo_atual, int colisao,
                 Som som) {
    if (bala_atual->ativa && inimigo_atual->ativo) {
        if (abs(bala_atual->x - inimigo_atual->posx) < colisao &&
            abs(bala_atual->y - inimigo_atual->posy) < colisao) {
            inimigo_atual->vida -= bala_atual->dano;
            bala_atual->ativa = false;
            al_play_sample(som.hit_inimigo, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
        }
    }
}

void processamentoBala(Inimigo inimigos[], int *indice, Bala balas[],
                       int *max_balas, int colisao, Jogador *canga, Som *sons) {
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
                    al_play_sample(sons->morte_inimigos, 0.5, 0, 1,
                                   ALLEGRO_PLAYMODE_ONCE, 0);
                    canga->xp += 1;
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

void danoJogador(Inimigo inimigos[], Jogador *canga, int indice, double counts,
                 Som som) {
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
        if (inimigos[i].comportamento == FORMIGA) {
            for (int j = 0; j < inimigos[i].quantidade_de_ataques; j++) {
                if (!inimigos[i].balas[j].ativa) {
                    continue;
                }

                if (abs(inimigos[i].balas[j].x - canga->x) < 25 &&
                    abs(inimigos[i].balas[j].y - canga->y) < 25 &&
                    counts - canga->ultimo_dano >= canga->dano_delay) {
                    canga->vida -= 1;
                    canga->ultimo_dano = counts;
                    inimigos[i].balas[j].ativa = false;
                    al_play_sample(som.hit, 2, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
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
    bool powerup_pendente;
    double coldoown_tatu;
    double coldoown_formiga;
    double ultima_wave;
    int contador_wave;
    int delay_mensagem;
    Som sons;
} EstadoGlobal;

/*
    Gera um novo estado de jogo. A função pede a folha de sprites para evitar
   carregar eles de novo todo restart.
*/
EstadoGlobal gerar_estado(FolhaSprites sprites, Som sons) {
    Jogador canga = {
        .sprite = sprites.canga,
        .sons = sons,
        .x = LARGURA / 2,
        .y = ALTURA / 2,

        .vida = 3,
        .vivo = true,
        .cooldown_arma = 30,
        .dano_delay = 1,
        .velocidade = 3,
        .dano = 1,
    };

    EstadoGlobal globs = {
        .sons = sons,
        .sprites = sprites,
        .canga = canga,

        /*
            Mesmo que todos esses já virem NULL por padrão, vou deixar eles
           documentados aqui pra ajudar a lembrar na hora de dar o free() no
           reiniciar_estado().
        */
        .balas = NULL,
        .homem_tatus = NULL,
        .formigas = NULL,
        .coldoown_tatu = 4,
        .coldoown_formiga = 6,
        .ultima_wave = 0,
        .contador_wave = 1,
        .delay_mensagem = 120,
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

    *antigo = gerar_estado(antigo->sprites, antigo->sons);
}

void reiniciar_inimigos(EstadoGlobal *globs) {
    free(globs->homem_tatus);
    free(globs->formigas);

    globs->indice_formiga = 0;
    globs->indice_tatu = 0;

    globs->homem_tatus = NULL;
    globs->formigas = NULL;
}

void waves(EstadoGlobal *globs) {
    double tempo_atual = al_get_time();
    // Coldoown por wave
    if (tempo_atual - globs->ultima_wave >= 25) {
        globs->contador_wave++;
        reiniciar_inimigos(globs);
        if (globs->coldoown_tatu > 0.5) {
            globs->coldoown_tatu -= 0.02 * (globs->contador_wave);
        }
        if (globs->coldoown_formiga > 0.5) {
            globs->coldoown_formiga -= 0.03 * (globs->contador_wave);
        }
        if (globs->coldoown_formiga < 0.5) {
            globs->coldoown_formiga = 0.5;
        }
        if (globs->coldoown_tatu < 0.5) {
            globs->coldoown_tatu = 0.5;
        }
        globs->delay_mensagem = 120;
        globs->ultima_wave = tempo_atual;
    }
}

// Adição para reiniciar as funções atualizadas da wave
void reiniciar_game(EstadoGlobal *globs) {
    reiniciar_estado(globs);
    globs->contador_wave = 1;
    globs->delay_mensagem = 120;
    globs->ultima_wave = al_get_time();
    globs->indice_formiga = 0;
    globs->indice_tatu = 0;
    globs->coldoown_tatu = 4;
    globs->coldoown_formiga = 6;
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
    al_install_audio();
    al_init_acodec_addon();
    al_reserve_samples(16);
    al_init_primitives_addon();
    al_install_keyboard();

    ALLEGRO_DISPLAY *tela = al_create_display(LARGURA, ALTURA);
    ALLEGRO_EVENT_QUEUE *fila = al_create_event_queue();
    al_register_event_source(fila, al_get_keyboard_event_source());
    al_register_event_source(fila, al_get_display_event_source(tela));
    ALLEGRO_FONT *fonte =
        al_load_ttf_font("./materiais/fontes/FiftiesMovies.ttf", 32, 0);
    ALLEGRO_FONT *fonte_power =
        al_load_ttf_font("./materiais/fontes/FiftiesMovies.ttf", 22, 0);

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
    //---------
    // Sons
    // ----------
    Som jogo_sons = {
        al_load_audio_stream("./materiais/sons/trilha_16bit.wav", 4, 2048),
        al_load_sample("./materiais/sons/morte_16bit.wav"),
        al_load_sample("./materiais/sons/disparo_16bit.wav"),
        al_load_sample("./materiais/sons/hit_16bit.wav"),
        al_load_audio_stream("./materiais/sons/derrota_16bit.wav", 4, 2048),
        al_load_sample("./materiais/sons/hitini_16bit.wav"),
    };
    // Inicialização das duas musicas, os outros efeitos de som estão expalhados
    // pelo código
    al_attach_audio_stream_to_mixer(jogo_sons.musica_de_fundo,
                                    al_get_default_mixer());
    al_set_audio_stream_gain(jogo_sons.musica_de_fundo, 0.6);
    al_set_audio_stream_playmode(jogo_sons.musica_de_fundo,
                                 ALLEGRO_PLAYMODE_LOOP);
    al_attach_audio_stream_to_mixer(jogo_sons.musica_derrota,
                                    al_get_default_mixer());
    al_set_audio_stream_playmode(jogo_sons.musica_derrota,
                                 ALLEGRO_PLAYMODE_LOOP);
    al_set_audio_stream_gain(jogo_sons.musica_derrota, 0.6);

    // ----------
    // Globais
    // ----------
    /*
        Eu criei essa struct só pra facilitar na hora do restart do jogo.
       Por mais que seja tentador, evite ao MÁÁÁXIMO passar esse cara como
       argumento de uma função. Vai evitar macarronada no código. Obrigado!
    */
    EstadoGlobal globs = gerar_estado(sprites, jogo_sons);

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
    // TODO: Deixar aleatório quando tiver mais do que 3
    EPowerUps powers_temp[3] = {AUMENTO_DANO, AUMENTO_VDA, AUMENTO_VDM};

    ALLEGRO_EVENT evento;
    for (;;) {
        al_wait_for_event(fila, &evento);

        capturar_movimento(evento, &globs.canga.movimento);
        capturar_mira(evento, &globs.canga.mira);

        if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            break;
        }

        if (!globs.canga.vivo) {
            redesenhar_mapa(sprites);
            al_set_audio_stream_playing(jogo_sons.musica_de_fundo, false);
            al_set_audio_stream_playing(jogo_sons.musica_derrota, true);

            al_draw_filled_rectangle(0, 0, LARGURA, ALTURA,
                                     al_map_rgba(25, 0, 0, 150));
            al_draw_filled_rectangle(0, (ALTURA / 2.0) - 80, LARGURA,
                                     (ALTURA / 2.0) + 80, al_map_rgb(0, 0, 0));

            al_draw_text(fonte, al_map_rgb(255, 255, 255), LARGURA / 2.0,
                         (ALTURA / 2.0) - 40, ALLEGRO_ALIGN_CENTER,
                         "SE LASCÔ!");
            al_draw_text(fonte, al_map_rgb(150, 150, 150), LARGURA / 2.0,
                         (ALTURA / 2.0) + 10, ALLEGRO_ALIGN_CENTER,
                         "Pressione [ESPAÇO] para recomeçar.");

            if (evento.keyboard.keycode == ALLEGRO_KEY_SPACE) {
                reiniciar_game(&globs);
            }

            al_flip_display();
            continue;
        }

        if (globs.canga.xp >= 20) {
            redesenhar_mapa(sprites);
            desenhar_powerups(powers_temp, fonte_power);

            if (evento.type == ALLEGRO_EVENT_KEY_DOWN) {
                switch (evento.keyboard.keycode) {
                case ALLEGRO_KEY_1:
                    aplicar_power(&globs.canga, powers_temp[0]);
                    globs.canga.xp = 0;
                    break;

                case ALLEGRO_KEY_2:
                    aplicar_power(&globs.canga, powers_temp[1]);
                    globs.canga.xp = 0;
                    break;

                case ALLEGRO_KEY_3:
                    aplicar_power(&globs.canga, powers_temp[2]);
                    globs.canga.xp = 0;
                    break;
                }
            }

            al_flip_display();
            continue;
        }

        if (evento.type == ALLEGRO_EVENT_TIMER) {
            criar_bala_jogador(&globs.balas, &globs.quant_balas, &globs.canga,
                               tick_timer, globs.sprites, globs.sons);

            al_set_audio_stream_playing(jogo_sons.musica_de_fundo, true);
            al_set_audio_stream_playing(jogo_sons.musica_derrota, false);
            waves(&globs);
            criar_bala_jogador(&globs.balas, &globs.quant_balas, &globs.canga,
                               tick_timer, globs.sprites, globs.sons);

            //--------
            // Inimigos
            //--------
            globs.counts = al_get_time();
            criarInimigo(&globs.homem_tatus, &globs.formigas, &globs.counts,
                         globs.sprites.formiga, globs.sprites.tatu,
                         &globs.ultimo_spawn_tatu, &globs.ultimo_spawn_formiga,
                         &globs.indice_tatu, &globs.indice_formiga,
                         &globs.coldoown_tatu, &globs.coldoown_formiga);

            inimigosLogica(globs.homem_tatus, &globs.indice_tatu, globs.canga,
                           &globs.counts, globs.sprites.cuspe);
            inimigosLogica(globs.formigas, &globs.indice_formiga, globs.canga,
                           &globs.counts, globs.sprites.cuspe);
            processamentoBala(globs.homem_tatus, &globs.indice_tatu,
                              globs.balas, &globs.quant_balas, 28, &globs.canga,
                              &globs.sons);
            processamentoBala(globs.formigas, &globs.indice_formiga,
                              globs.balas, &globs.quant_balas, 22, &globs.canga,
                              &globs.sons);
            danoJogador(globs.homem_tatus, &globs.canga, globs.indice_tatu,
                        globs.counts, globs.sons);
            danoJogador(globs.formigas, &globs.canga, globs.indice_formiga,
                        globs.counts, globs.sons);

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
            if (globs.delay_mensagem > 0) {
                al_draw_textf(fonte, al_map_rgb(255, 255, 255), LARGURA / 2.0,
                              ALTURA / 2.0 - 200, ALLEGRO_ALIGN_CENTER,
                              "WAVE %d", globs.contador_wave);
                globs.delay_mensagem--;
            }
            // al_draw_filled_circle(canga.x, canga.y, 5, al_map_rgb(255, 0,
            // 0));

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
    al_destroy_audio_stream(jogo_sons.musica_de_fundo);
    al_destroy_audio_stream(jogo_sons.musica_derrota);
    // free(formigas);
    // free(homem_tatus);

    return 0;
}