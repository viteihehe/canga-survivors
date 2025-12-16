#include "constantes.h"
#include "logica/cenario.h"
#include "logica/inimigos.h"
#include "logica/jogador.h"
#include "logica/powerups.h"
#include "logica/telas.h"
#include "midia.h"
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

    Bala *balas;
    int quant_balas;

    Inimigo *inimigos;
    int quant_inim;
    double ultimo_spawn_inim;
    double coldoown_inim;

    double counts;

    int total_inimigos_wave;
    int inimigos_mortos;
    bool wave_ativa;
    int contador_wave;
    double ultima_wave;
    int delay_mensagem;
    int maximo_inims;
    Estatisticas estatisticas;
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
        .frame_atual = 7,
        .contador_frame = 0,
        .frame_pernas = 1,
    };

    Estatisticas est = {
        .Dano_causado = 0,
        .Dano_sofrido = 0,
        .nivel_atingido = 0,
        .passos_dados = 0,
        .tempo_de_jogo = 0,
        .pontuacao_run = 0,
        .total_inimigos_mortos = 0
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
        .inimigos = NULL,
        .coldoown_inim = 2,
        .total_inimigos_wave = 5,
        .inimigos_mortos = 0,
        .delay_mensagem = 120,
        .wave_ativa = true,
        .contador_wave = 1,
        .ultima_wave = 0,
        .maximo_inims = 0,
        .estatisticas = est
    };

    return globs;
}

/*
    Reinicia o estado de um jogo anterior.
*/
void reiniciar_estado(EstadoGlobal *antigo) {

    free(antigo->inimigos);

    *antigo = gerar_estado(antigo->sprites, antigo->sons);
}

void reiniciar_inimigos(EstadoGlobal *globs) {
    free(globs->inimigos);

    globs->quant_inim = 0;
    globs->inimigos = NULL;
}

void waves(EstadoGlobal *globs, EPowerUps *powers) {
    if (globs->wave_ativa &&
        globs->inimigos_mortos >= globs->total_inimigos_wave) {

        aleatorizar_powers(powers);
        globs->canga.powerup_pronto = true;
        globs->wave_ativa = false;
        globs->ultima_wave = al_get_time();
    }
    if (!globs->wave_ativa && al_get_time() - globs->ultima_wave >= 1) {
        globs->contador_wave++;
        globs->total_inimigos_wave += 2;
        globs->inimigos_mortos = 0;
        globs->maximo_inims = 0;
        globs->wave_ativa = true;
        globs->delay_mensagem = 120;

        reiniciar_inimigos(globs);
        if (globs->coldoown_inim >= 0.5) {
            globs->coldoown_inim -= 0.15;
        }
    }
}

typedef enum {
    B_JOGAR,
    B_PONTUACAO,
    B_CREDITOS,
    B_SAIR,
} EBotaoMenu;

/*
    Redesenha o menu principal.
*/
void desenhar_menu(
    int *indice_botao_ativo,
    FolhaSprites sprites,
    Som sons,
    ALLEGRO_FONT *fonte_menu,
    ALLEGRO_FONT *fonte_botao,
    ALLEGRO_EVENT evento
) {
    desenhar_mapa(sprites);

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
        "Pontuação",
        COR_BRANCO,
        LARGURA / 2,
        ALTURA / 2 + 80,
        300,
        70,
        fonte_botao
    );

    desenhar_caixa_texto(
        "Créditos",
        COR_BRANCO,
        LARGURA / 2,
        ALTURA / 2 + 160,
        300,
        70,
        fonte_botao
    );

    desenhar_caixa_texto(
        "Sair", COR_BRANCO, LARGURA / 2, ALTURA / 2 + 240, 300, 70, fonte_botao
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

    al_hide_mouse_cursor(tela);

    // ----------
    // Fontes
    // ----------
    ALLEGRO_FONT *fonte =
        al_load_ttf_font("./materiais/fontes/FiftiesMovies.ttf", 32, 0);

    ALLEGRO_FONT *fonte_titulo =
        al_load_ttf_font("./materiais/fontes/FiftiesMovies.ttf", 70, 0);
    ALLEGRO_FONT *fonte_menor =
        al_load_ttf_font("./materiais/fontes/FiftiesMovies.ttf", 25, 0);

    ALLEGRO_FONT *fonte_frase =
        al_load_ttf_font("./materiais/fontes/FiftiesMovies.ttf", 50, 0);

    // ----------
    // Sprites
    // ----------
    FolhaSprites sprites = {
        .canga = al_load_bitmap("./materiais/sprites/canga_com.png"),
        .coracao = al_load_bitmap("./materiais/sprites/coracao.png"),

        .guia_wasd = al_load_bitmap("./materiais/sprites/wasd.png"),
        .guia_setas = al_load_bitmap("./materiais/sprites/setas.png"),

        .tatu = al_load_bitmap("./materiais/sprites/peba2_1.png"),
        .formiga = al_load_bitmap("./materiais/sprites/formiga.png"),
        .cuspe = al_load_bitmap("./materiais/sprites/cuspe.png"),

        .bala = al_load_bitmap("./materiais/sprites/bala.png"),
        .sombra = al_load_bitmap("./materiais/sprites/sombra.png"),

        .areia = al_load_bitmap("./materiais/sprites/mapa/areia.png"),
        .cacto = al_load_bitmap("./materiais/sprites/mapa/cacto.png"),
        .pedra = al_load_bitmap("./materiais/sprites/mapa/pedra.png"),
        .arbusto = al_load_bitmap("./materiais/sprites/mapa/arbusto.png"),

        .grama = al_load_bitmap("./materiais/sprites/mapa/grama.png"),
        .pedrinhas = al_load_bitmap("./materiais/sprites/mapa/pedrinhas.png"),
        .canga_pernas = al_load_bitmap("./materiais/sprites/pernas.png"),
        .caveira = al_load_bitmap("./materiais/sprites/caveira.png"),
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
    bool jogo_pausado = false;
    bool usuario_na_pontuacao = false;
    bool usuario_nos_creditos = false;
    bool usuario_no_menu = true;
    int botao_menu_selecionado = 0;
    char letra = 'A';
    int aux = 0;
    int tempo = 10;
    char sigla[4] = {'_', '_', '_', '\0'};
    bool selecionou = false;
    bool gravar = true;
    bool jogador_primeiro_mov = false;

    EPowerUps *powers = calloc(sizeof(EPowerUps), 3);

    ALLEGRO_EVENT evento;
    for (;;) {
        al_wait_for_event(fila, &evento);

        if (globs.canga.vivo && !usuario_no_menu) {
            capturar_movimento(
                evento,
                &globs.canga.movimento,
                &globs.estatisticas.passos_dados,
                &jogador_primeiro_mov
            );
            capturar_mira(evento, &globs.canga.mira);
        }

        // ----------
        // Controle de Janela
        // ----------
        if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE || forcar_fechamento) {
            break;
        }

        // ----------
        // Tela de Créditos
        // ----------
        if (usuario_nos_creditos) {
            desenhar_mapa(sprites);

            desenhar_caixa_texto(
                "Criado por Jozenias Antônio e Victor Manuel",
                COR_BRANCO,
                LARGURA / 2,
                ALTURA / 2,
                750,
                120,
                fonte
            );

            if (evento.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
                usuario_nos_creditos = false;
            }

            al_flip_display();
            continue;
        }

        // ----------
        // Menu Principal
        // ----------
        if (usuario_no_menu) {
            al_set_audio_stream_playing(jogo_sons.menu, true);
            al_set_audio_stream_playing(jogo_sons.musica_derrota, false);
            al_set_audio_stream_playing(jogo_sons.musica_de_fundo, false);

            if (evento.type == ALLEGRO_EVENT_KEY_DOWN) {
                switch (evento.keyboard.keycode) {
                case ALLEGRO_KEY_W:
                case ALLEGRO_KEY_UP:
                    ciclar_inteiro(&botao_menu_selecionado, 0, 3, -1);

                    al_play_sample(
                        jogo_sons.selecao, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0
                    );
                    break;

                case ALLEGRO_KEY_S:
                case ALLEGRO_KEY_DOWN:
                    ciclar_inteiro(&botao_menu_selecionado, 0, 3, +1);

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
                        carregar_mapa_jogo();
                        al_set_audio_stream_playing(jogo_sons.menu, false);
                        al_set_audio_stream_playing(
                            jogo_sons.musica_de_fundo, true
                        );
                        break;

                    case B_PONTUACAO:
                        usuario_no_menu = false;
                        usuario_na_pontuacao = true;
                        al_flush_event_queue(fila);
                        break;

                    case B_CREDITOS:
                        usuario_nos_creditos = true;
                        break;

                    case B_SAIR:
                        forcar_fechamento = true;
                        break;
                    }
                }
            }

            desenhar_menu(
                &botao_menu_selecionado,
                globs.sprites,
                jogo_sons,
                fonte_titulo,
                fonte,
                evento
            );
            continue;
        }

        // ----------
        // Tela de Game Over
        // ----------
        if (!globs.canga.vivo) {
            if (tempo > 0) {
                letra = 'A';
                aux = 0;
                selecionou = false;
                globs.estatisticas.nivel_atingido = globs.contador_wave - 1;
                globs.estatisticas.pontuacao_run = globs.canga.pontuacao;
            }
            if (tempo != 0) {
                tempo--;
            }
            desenhar_mapa(sprites);
            al_set_audio_stream_playing(jogo_sons.musica_de_fundo, false);
            al_set_audio_stream_playing(jogo_sons.musica_derrota, true);

            if (gravar) {

                tela_morte(
                    evento,
                    globs.canga.pontuacao,
                    fonte_menor,
                    fonte,
                    fonte_frase,
                    sigla,
                    &letra,
                    &aux,
                    &selecionou,
                    jogo_sons.escolha,
                    jogo_sons.selecao,
                    globs.sprites.caveira,
                    globs.estatisticas
                );

                if (aux == 3) {
                    salvar_arquivo(globs.canga.pontuacao, sigla);
                    gravar = false;
                }
            } else {
                tela_morte(
                    evento,
                    globs.canga.pontuacao,
                    fonte_menor,
                    fonte,
                    fonte_frase,
                    sigla,
                    &letra,
                    &aux,
                    &selecionou,
                    jogo_sons.escolha,
                    jogo_sons.selecao,
                    globs.sprites.caveira,
                    globs.estatisticas
                );
                desenhar_caixa_texto(
                    "Aperte [espaço] para recomeçar",
                    COR_BRANCO,
                    LARGURA / 2,
                    700,
                    500,
                    80,
                    fonte
                );
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
                tempo = 10;
                al_set_audio_stream_playing(jogo_sons.musica_derrota, false);
                al_set_audio_stream_playing(jogo_sons.musica_de_fundo, true);
            }

            al_flip_display();
            continue;
        }

        // ----------
        // Tela de Pontuações
        // ----------
        if (usuario_na_pontuacao) {
            char sigla_busca[4];
            if (tempo > 0) {
                aux = 0;
                selecionou = false;
                letra = 'A';
                evento.keyboard.keycode = 0;
                strcpy(sigla_busca, "___");
            }
            if (tempo != 0) {
                tempo--;
            }
            al_set_audio_stream_playing(jogo_sons.musica_de_fundo, false);
            al_set_audio_stream_playing(jogo_sons.musica_derrota, false);

            busca_pontucao(
                evento,
                fonte,
                fonte_titulo,
                &aux,
                &selecionou,
                &letra,
                jogo_sons.escolha,
                jogo_sons.selecao,
                sigla_busca
            );

            al_flip_display();

            if (evento.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
                usuario_no_menu = true;
                usuario_na_pontuacao = false;
                tempo = 10;
            }
            continue;
        }

        // ----------
        // Tela de Powerup
        // ----------
        if (globs.canga.powerup_pronto) {
            desenhar_mapa(sprites);
            desenhar_powerups(powers, fonte);

            if (evento.type == ALLEGRO_EVENT_KEY_DOWN) {
                switch (evento.keyboard.keycode) {
                case ALLEGRO_KEY_1:
                    aplicar_power(&globs.canga, powers[0]);
                    globs.canga.powerup_pronto = false;
                    al_flush_event_queue(fila);
                    break;

                case ALLEGRO_KEY_2:
                    aplicar_power(&globs.canga, powers[1]);
                    globs.canga.powerup_pronto = false;
                    al_flush_event_queue(fila);
                    break;

                case ALLEGRO_KEY_3:
                    aplicar_power(&globs.canga, powers[2]);
                    globs.canga.powerup_pronto = false;
                    al_flush_event_queue(fila);
                    break;
                }
            }

            al_flip_display();
            continue;
        }

        // ----------
        // Tela de Pause
        // ----------
        if (evento.type == ALLEGRO_EVENT_KEY_DOWN &&
            evento.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
            jogo_pausado = !jogo_pausado;
            al_flush_event_queue(fila);
        }

        else if (evento.type == ALLEGRO_EVENT_DISPLAY_SWITCH_OUT) {
            jogo_pausado = true;
            al_flush_event_queue(fila);
        }

        if (jogo_pausado) {
            desenhar_mapa(sprites);

            // Fundo
            al_draw_filled_rectangle(
                0, 0, LARGURA, ALTURA, al_map_rgba(0, 0, 0, 150)
            );

            // Barra
            al_draw_filled_rectangle(
                0, (ALTURA / 2.0) - 80, LARGURA, (ALTURA / 2.0) + 80, COR_PRETO
            );

            al_draw_text(
                fonte,
                COR_BRANCO,
                LARGURA / 2.0,
                (ALTURA / 2.0) - (al_get_font_ascent(fonte) / 2.0) - 25,
                ALLEGRO_ALIGN_CENTRE,
                "Pausado"
            );

            al_draw_text(
                fonte,
                al_map_rgb(150, 150, 150),
                LARGURA / 2.0,
                (ALTURA / 2.0) - (al_get_font_ascent(fonte) / 2.0) + 25,
                ALLEGRO_ALIGN_CENTRE,
                "Pressione [ESC] para despausar"
            );

            al_flip_display();
            continue;
        }

        // ----------
        // Lógica Principal
        // ----------
        if (evento.type == ALLEGRO_EVENT_TIMER) {
            criar_bala_jogador(
                &globs.canga, tick_timer, globs.sprites, globs.sons
            );

            waves(&globs, powers);
            criar_bala_jogador(
                &globs.canga, tick_timer, globs.sprites, globs.sons
            );

            // --------
            // Inimigos
            // --------
            globs.counts = al_get_time();

            if (globs.maximo_inims < globs.total_inimigos_wave) {
                criarInimigo(
                    &globs.inimigos,
                    globs.sprites,
                    &globs.ultimo_spawn_inim,
                    &globs.quant_inim,
                    &globs.coldoown_inim,
                    rand() % 2,
                    &globs.maximo_inims
                );
            }

            inimigosLogica(
                globs.inimigos, &globs.quant_inim, globs.canga, &globs.counts
            );

            processamentoBala(
                globs.inimigos,
                &globs.quant_inim,
                &globs.canga.balas,
                28,
                &globs.canga,
                &globs.sons,
                &globs.inimigos_mortos,
                &globs.estatisticas.Dano_causado,
                &globs.estatisticas.total_inimigos_mortos
            );
            danoJogador(
                globs.inimigos,
                &globs.canga,
                globs.quant_inim,
                globs.counts,
                globs.sons,
                &globs.estatisticas.Dano_sofrido
            );

            // ----------
            // Redesenho
            // ----------
            desenhar_mapa(sprites);
            mover_jogador(globs.canga.movimento, &globs.canga);
            desenhar_jogador(&globs.canga, globs.sprites.canga_pernas);
            desenharInimigo(globs.inimigos, globs.quant_inim, globs.canga);
            mover_balas(&globs.canga.balas);
            desenhar_vida_jogador(&globs.canga, globs.sprites);
            desenhar_vida_inimigos(globs.inimigos, globs.quant_inim);
            desenhar_pontuacao(globs.canga.pontuacao, fonte);

            if (globs.delay_mensagem > 0) {
                char mensagem_wave[30];
                sprintf(mensagem_wave, "Wave %i", globs.contador_wave);

                desenhar_caixa_texto(
                    mensagem_wave, COR_BRANCO, LARGURA / 2, 120, 220, 60, fonte
                );

                globs.delay_mensagem--;
            }

            if (globs.estatisticas.passos_dados == 0) {
                desenhar_guias(sprites, fonte_menor);
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