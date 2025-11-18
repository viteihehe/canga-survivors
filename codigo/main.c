#include "constantes.h"
#include "logica/cenario.h"
#include "logica/inimigos.h"
#include "logica/jogador.h"
#include "logica/powerups.h"
#include "logica/tela_morte.h"
#include "utils.h"
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

    Inimigo *homem_tatus;
    int indice_tatu;
    double ultimo_spawn_tatu;
    double coldoown_tatu;

    Inimigo *formigas;
    int indice_formiga;
    double ultimo_spawn_formiga;
    double coldoown_formiga;

    double counts;

    int total_inimigos_wave;
    int inimigos_mortos;
    bool wave_ativa;
    int contador_wave;
    double ultima_wave;
    int delay_mensagem;
    int maximo_inimigos;
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
        .dano_delay = 2,
        .velocidade = 3,
        .dano = 30,
        .pontuacao = 0,
        .balas.inicio = NULL,
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
        .homem_tatus = NULL,
        .formigas = NULL,
        .coldoown_tatu = 4,
        .coldoown_formiga = 6,
        .total_inimigos_wave = 5,
        .inimigos_mortos = 0,
        .delay_mensagem = 120,
        .wave_ativa = true,
        .contador_wave = 1,
        .ultima_wave = 0,
        .maximo_inimigos = 0,
    };

    return globs;
}

/*
    Reinicia o estado de um jogo anterior.
*/
void reiniciar_estado(EstadoGlobal *antigo) {
  
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
    if (globs->wave_ativa &&
        globs->inimigos_mortos >= globs->total_inimigos_wave) {

        globs->canga.powerup_pronto = true;
        globs->wave_ativa = false;
        globs->ultima_wave = al_get_time();
    }
    if (!globs->wave_ativa && al_get_time() - globs->ultima_wave >= 1) {
        globs->contador_wave++;
        globs->total_inimigos_wave += 2;
        globs->inimigos_mortos = 0;
        globs->maximo_inimigos = 0;
        globs->wave_ativa = true;
        globs->delay_mensagem = 120;

        reiniciar_inimigos(globs);
        if (globs->coldoown_tatu >= 0.5) {
            globs->coldoown_tatu -= 0.1;
        }
        if (globs->coldoown_formiga >= 0.5) {
            globs->coldoown_formiga -= 0.1;
        }
    }
}

typedef enum {
    B_JOGAR,
    B_CREDITOS,
    B_SAIR,
} EBotaoMenu;

/*
    Redesenha o menu principal.
*/
void desenhar_menu(
    int *indice_botao_ativo,
    ALLEGRO_BITMAP *fundo,
    Som sons,
    ALLEGRO_FONT *fonte_menu,
    ALLEGRO_FONT *fonte_botao,
    ALLEGRO_EVENT evento
) {
    al_draw_bitmap(fundo, 0, 0, 0);

    // Título
    desenhar_caixa_texto(
        "CANGA SURVIVORS",
        COR_BRANCO,
        LARGURA / 2,
        ALTURA / 2 - 150,
        620,
        120,
        fonte_menu
    );

    // Botões
    desenhar_caixa_texto(
        "Jogar", COR_BRANCO, LARGURA / 2, ALTURA / 2, 300, 70, fonte_botao
    );

    desenhar_caixa_texto(
        "Créditos",
        COR_BRANCO,
        LARGURA / 2,
        ALTURA / 2 + 80,
        300,
        70,
        fonte_botao
    );

    desenhar_caixa_texto(
        "Sair", COR_BRANCO, LARGURA / 2, ALTURA / 2 + 160, 300, 70, fonte_botao
    );

    // Selecionador
    desenhar_caixa_texto(
        "->",
        COR_BRANCO,
        LARGURA / 2 - 200,
        ALTURA / 2 + (80 * *indice_botao_ativo),
        50,
        50,
        fonte_botao
    );

    al_flip_display();
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
    srand(time(NULL));

    ALLEGRO_DISPLAY *tela = al_create_display(LARGURA, ALTURA);
    ALLEGRO_EVENT_QUEUE *fila = al_create_event_queue();
    al_register_event_source(fila, al_get_keyboard_event_source());
    al_register_event_source(fila, al_get_display_event_source(tela));

    ALLEGRO_TIMER *tick_timer = al_create_timer(1.0 / FPS);
    al_register_event_source(fila, al_get_timer_event_source(tick_timer));
    al_start_timer(tick_timer);

    // ----------
    // Fontes
    // ----------
    ALLEGRO_FONT *fonte =
        al_load_ttf_font("./materiais/fontes/FiftiesMovies.ttf", 32, 0);

    ALLEGRO_FONT *fonte_titulo =
        al_load_ttf_font("./materiais/fontes/FiftiesMovies.ttf", 70, 0);

    ALLEGRO_BITMAP *menu_sprite =
        al_load_bitmap("./materiais/sprites/menu2.png");

    // ----------
    // Sprites
    // ----------
    FolhaSprites sprites = {
        .canga = al_load_bitmap("./materiais/sprites/canga.png"),
        .coracao = al_load_bitmap("./materiais/sprites/coracao.png"),

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
        al_load_audio_stream(
            "./materiais/sons/trilhasonora_16bit.wav", 4, 2048
        ),
        al_load_sample("./materiais/sons/morte_16bit.wav"),
        al_load_sample("./materiais/sons/disparo_16bit.wav"),
        al_load_sample("./materiais/sons/hit_16bit.wav"),
        al_load_audio_stream("./materiais/sons/derrota_16bit.wav", 4, 2048),
        al_load_sample("./materiais/sons/hitini_16bit.wav"),
        al_load_audio_stream("./materiais/sons/menu_16bit.wav", 4, 2048),
        al_load_sample("./materiais/sons/som_menu_16bit.wav"),
        al_load_sample("./materiais/sons/escolha_16bit.wav"),
        al_load_sample("./materiais/sons/texto_16bit.wav")
    };

    al_attach_audio_stream_to_mixer(jogo_sons.menu, al_get_default_mixer());
    al_set_audio_stream_gain(jogo_sons.menu, 0.6);
    al_set_audio_stream_playmode(jogo_sons.menu, ALLEGRO_PLAYMODE_LOOP);

    al_attach_audio_stream_to_mixer(
        jogo_sons.musica_de_fundo, al_get_default_mixer()
    );
    al_set_audio_stream_gain(jogo_sons.musica_de_fundo, 0.6);
    al_set_audio_stream_playmode(
        jogo_sons.musica_de_fundo, ALLEGRO_PLAYMODE_LOOP
    );
    al_attach_audio_stream_to_mixer(
        jogo_sons.musica_derrota, al_get_default_mixer()
    );
    al_set_audio_stream_playmode(
        jogo_sons.musica_derrota, ALLEGRO_PLAYMODE_LOOP
    );
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
    al_set_window_title(tela, "Canga Survivors");

    // ----------
    // Loop Principal
    // ----------
    bool forcar_fechamento = false;
    bool usuario_no_menu = true;
    int botao_menu_selecionado = 0;
    char letra = 'A';
    int aux = 0;
    char sigla[4] = {'_', '_', '_', '\0'};
    bool selecionou = false;
    bool gravar = true;

    // TODO: Deixar aleatório quando tiver mais do que 3
    EPowerUps powers_temp[3] = {AUMENTO_DANO, AUMENTO_VDA, AUMENTO_VDM};

    ALLEGRO_EVENT evento;
    for (;;) {
        al_wait_for_event(fila, &evento);

        // ----------
        // Fechamento
        // ----------
        if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE || forcar_fechamento) {
            break;
        }

        // ----------
        // Menu Principal
        // ----------
        if (usuario_no_menu) {

            al_set_audio_stream_playing(jogo_sons.musica_derrota, false);
            al_set_audio_stream_playing(jogo_sons.musica_de_fundo, false);
            al_set_audio_stream_playing(jogo_sons.menu, true);

            if (evento.type == ALLEGRO_EVENT_KEY_DOWN) {
                switch (evento.keyboard.keycode) {
                case ALLEGRO_KEY_W:
                case ALLEGRO_KEY_UP:
                    ciclar_inteiro(&botao_menu_selecionado, 0, 2, -1);

                    al_play_sample(
                        jogo_sons.selecao, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0
                    );
                    break;

                case ALLEGRO_KEY_S:
                case ALLEGRO_KEY_DOWN:
                    ciclar_inteiro(&botao_menu_selecionado, 0, 2, +1);

                    al_play_sample(
                        jogo_sons.selecao, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0
                    );
                    break;

                case ALLEGRO_KEY_SPACE:
                case ALLEGRO_KEY_ENTER:
                    if (botao_menu_selecionado != B_SAIR) {
                        al_play_sample(
                            jogo_sons.escolha, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0
                        );
                    }

                    switch (botao_menu_selecionado) {
                    case B_JOGAR:
                        usuario_no_menu = false;
                        break;

                    case B_CREDITOS:
                        // TODO: Criar painél de créditos;
                        break;

                    case B_SAIR:
                        forcar_fechamento = true;
                        break;
                    }
                }
            }

            desenhar_menu(
                &botao_menu_selecionado,
                menu_sprite,
                jogo_sons,
                fonte_titulo,
                fonte,
                evento
            );
            continue;
        }

        // FIXME: Esses sons não eram pra ser executados toda vez
        al_set_audio_stream_playing(jogo_sons.menu, false);
        al_set_audio_stream_playing(jogo_sons.musica_de_fundo, true);

        capturar_movimento(evento, &globs.canga.movimento);
        capturar_mira(evento, &globs.canga.mira);

        // ----------
        // Tela de Game Over
        // ----------
        if (!globs.canga.vivo) {
            desenhar_mapa(sprites);
            al_set_audio_stream_playing(jogo_sons.musica_de_fundo, false);
            al_set_audio_stream_playing(jogo_sons.musica_derrota, true);

            // al_draw_filled_rectangle(
            //     0, 0, LARGURA, ALTURA, al_map_rgba(25, 0, 0, 150)
            // );
            // al_draw_filled_rectangle(
            //     0, (ALTURA / 2.0) - 80, LARGURA, (ALTURA / 2.0) + 80,
            //     COR_PRETO
            // );

            // al_draw_text(
            //     fonte,
            //     COR_BRANCO,
            //     LARGURA / 2.0,
            //     (ALTURA / 2.0) - 40,
            //     ALLEGRO_ALIGN_CENTER,
            //     "SE LASCÔ!"
            // );
            // al_draw_text(
            //     fonte,
            //     al_map_rgb(150, 150, 150),
            //     LARGURA / 2.0,
            //     (ALTURA / 2.0) + 10,
            //     ALLEGRO_ALIGN_CENTER,
            //     "Pressione [ESPAÇO] para recomeçar."
            // );
            if (gravar) {
                tela_morte(
                    globs.canga.pontuacao,
                    fonte_titulo,
                    fonte,
                    sigla,
                    &letra,
                    &aux,
                    &selecionou
                );
                exibir_lista(fonte, fonte_titulo);
                if (aux == 3) {
                    salvar_arquivo(globs.canga.pontuacao, sigla);
                    gravar = false;
                }
            } else {
                al_draw_filled_rectangle(180, -10, 820, 800, COR_PRETO);
                exibir_lista(fonte, fonte_titulo);
                al_draw_text(
                    fonte,
                    COR_BRANCO,
                    LARGURA/2,
                    700,
                    ALLEGRO_ALIGN_CENTRE,
                    "Aperte [espaço] para recomeçar"
                );
            }

            if (evento.type == ALLEGRO_EVENT_KEY_UP) {
                if (evento.keyboard.keycode == ALLEGRO_KEY_S ||
                    evento.keyboard.keycode == ALLEGRO_KEY_DOWN) {
                    if (letra > '@' && letra < '[') {
                        letra += 1;
                    } else {
                        letra = 'A';
                    }
                    al_play_sample(
                        jogo_sons.selecao, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0
                    );
                }
                if (evento.keyboard.keycode == ALLEGRO_KEY_W ||
                    evento.keyboard.keycode == ALLEGRO_KEY_UP) {
                    if (letra > '@' && letra < '[') {
                        letra -= 1;
                    } else {
                        letra = 'A';
                    }
                    al_play_sample(
                        jogo_sons.selecao, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0
                    );
                }

                if ((evento.keyboard.keycode) == ALLEGRO_KEY_ENTER) {
                    selecionou = true;
                }
            }

            if ((evento.keyboard.keycode) == ALLEGRO_KEY_SPACE) {
                reiniciar_estado(&globs);
                botao_menu_selecionado = 0;
                letra = 'A';
                aux = 0;
                sigla[0] = '_';
                sigla[1] = '_';
                sigla[2] = '_';
                sigla[3] = '\0';
                selecionou = false;
                gravar = true;
            }

            al_flip_display();
            continue;
        }

        // ----------
        // Tela de Powerup
        // ----------
        if (globs.canga.powerup_pronto) {
            desenhar_mapa(sprites);
            desenhar_powerups(powers_temp, fonte);

            if (evento.type == ALLEGRO_EVENT_KEY_DOWN) {
                switch (evento.keyboard.keycode) {
                case ALLEGRO_KEY_1:
                    aplicar_power(&globs.canga, powers_temp[0]);
                    globs.canga.powerup_pronto = false;
                    break;

                case ALLEGRO_KEY_2:
                    aplicar_power(&globs.canga, powers_temp[1]);
                    globs.canga.powerup_pronto = false;
                    break;

                case ALLEGRO_KEY_3:
                    aplicar_power(&globs.canga, powers_temp[2]);
                    globs.canga.powerup_pronto = false;
                    break;
                }
            }

            al_flip_display();
            continue;
        }

        // ----------
        // Lógica Principal
        // ----------
        if (evento.type == ALLEGRO_EVENT_TIMER) {
            criar_bala_jogador(
                &globs.canga,
                tick_timer,
                globs.sprites,
                globs.sons
            );

            al_set_audio_stream_playing(jogo_sons.musica_de_fundo, true);
            al_set_audio_stream_playing(jogo_sons.musica_derrota, false);
            waves(&globs);
            criar_bala_jogador(
                &globs.canga,
                tick_timer,
                globs.sprites,
                globs.sons
            );

            // --------
            // Inimigos
            // --------
            globs.counts = al_get_time();

            if (globs.maximo_inimigos < globs.total_inimigos_wave) {
                int tipo = rand() % 2;
                criarInimigo(
                    &globs.homem_tatus,
                    &globs.formigas,
                    &globs.counts,
                    globs.sprites.formiga,
                    globs.sprites.tatu,
                    &globs.ultimo_spawn_tatu,
                    &globs.ultimo_spawn_formiga,
                    &globs.indice_tatu,
                    &globs.indice_formiga,
                    &globs.coldoown_tatu,
                    &globs.coldoown_formiga,
                    tipo,
                    &globs.maximo_inimigos
                );
            }

            // printf(
            //   "Wave: %d | Mortos: %d/%d | Max: %d | Ativa: %d\n",
            //   globs.contador_wave,
            //   globs.inimigos_mortos,
            //   globs.total_inimigos_wave,
            //   globs.maximo_inimigos,
            //   globs.wave_ativa
            // );
            // Caso queira ver a wave funcionando

            inimigosLogica(
                globs.homem_tatus,
                &globs.indice_tatu,
                globs.canga,
                &globs.counts,
                globs.sprites.cuspe
            );
            inimigosLogica(
                globs.formigas,
                &globs.indice_formiga,
                globs.canga,
                &globs.counts,
                globs.sprites.cuspe
            );
            processamentoBala(
                globs.homem_tatus,
                &globs.indice_tatu,
                &globs.canga.balas,
                28,
                &globs.canga,
                &globs.sons,
                &globs.inimigos_mortos
            );
            processamentoBala(
                globs.formigas,
                &globs.indice_formiga,
                &globs.canga.balas,
                22,
                &globs.canga,
                &globs.sons,
                &globs.inimigos_mortos
            );
            danoJogador(
                globs.homem_tatus,
                &globs.canga,
                globs.indice_tatu,
                globs.counts,
                globs.sons
            );
            danoJogador(
                globs.formigas,
                &globs.canga,
                globs.indice_formiga,
                globs.counts,
                globs.sons
            );

            // ----------
            // Redesenho
            // ----------
            desenhar_mapa(sprites);
            mover_jogador(globs.canga.movimento, &globs.canga);
            desenharInimigo(globs.homem_tatus, globs.indice_tatu, globs.canga);
            desenharInimigo(globs.formigas, globs.indice_formiga, globs.canga);
            mover_balas(&globs.canga.balas);
            desenhar_vida_jogador(&globs.canga, globs.sprites);
            desenhar_vida_inimigos(globs.homem_tatus, globs.indice_tatu);
            desenhar_vida_inimigos(globs.formigas, globs.indice_formiga);
            desenhar_pontuacao(globs.canga.pontuacao, fonte);

            if (globs.delay_mensagem > 0) {
                char mensagem_wave[30];
                sprintf(mensagem_wave, "Wave %i", globs.contador_wave);

                desenhar_caixa_texto(
                    mensagem_wave, COR_BRANCO, LARGURA / 2, 120, 220, 60, fonte
                );

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
    al_destroy_audio_stream(jogo_sons.menu);
    liberar_lista(&globs.canga.balas);

    return 0;
}