#include "constantes.h"
#include "logica/cenario.h"
#include "logica/inimigos.h"
#include "logica/jogador.h"
#include "logica/powerups.h"
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
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct {
    FolhaSprites sprites;
    Som sons;

    Jogador canga;
    bool powerup_pendente;

    Bala *balas;
    int quant_balas;

    Inimigo *homem_tatus;
    int indice_tatu;
    double ultimo_spawn_tatu;
    double coldoown_tatu;

    Inimigo *formigas;
    int indice_formiga;
    double ultimo_spawn_formiga;
    double coldoown_formiga;

    double counts;
    int contador_frames;

    double ultima_wave;
    int contador_wave;
    int delay_mensagem;
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
        .cooldown_arma = 60,
        .dano_delay = 1,
        .velocidade = 3,
        .dano = 30,
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

    ALLEGRO_FONT *fonte_powers =
        al_load_ttf_font("./materiais/fontes/FiftiesMovies.ttf", 23, 0);

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

    // ---------
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
                reiniciar_estado(&globs);
            }

            al_flip_display();
            continue;
        }

        if (globs.canga.xp >= 1) {
            redesenhar_mapa(sprites);
            desenhar_powerups(powers_temp, fonte_powers);

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

            // --------
            // Inimigos
            // --------
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
            // Redesenho
            // ----------
            al_draw_filled_rectangle(0, 0, LARGURA, ALTURA,
                                     al_map_rgb(0, 0, 0));
            redesenhar_mapa(globs.sprites);

            mover_jogador(globs.canga.movimento, &globs.canga);
            desenharInimigo(globs.homem_tatus, globs.indice_tatu,
                            &globs.contador_frames, globs.canga);
            desenharInimigo(globs.formigas, globs.indice_formiga,
                            &globs.contador_frames, globs.canga);
            mover_balas(globs.balas, globs.quant_balas);
            if (globs.delay_mensagem > 0) {
                al_draw_textf(fonte, al_map_rgb(255, 255, 255), LARGURA / 2.0,
                              ALTURA / 2.0 - 200, ALLEGRO_ALIGN_CENTER,
                              "WAVE %d", globs.contador_wave);
                globs.delay_mensagem--;
            }
            al_flip_display();
        }
    }

    al_destroy_display(tela);
    al_destroy_timer(tick_timer);
    al_destroy_event_queue(fila);
    al_destroy_font(fonte);
    al_destroy_audio_stream(jogo_sons.musica_de_fundo);
    al_destroy_audio_stream(jogo_sons.musica_derrota);

    return 0;
}