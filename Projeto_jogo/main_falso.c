#include <allegro5/allegro5.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <stdio.h> // CORREÇÃO: Necessário para snprintf

#define LARGURA 1280
#define ALTURA 720
#define MAX_BALAS 2000

// Caminho absoluto para o seu sistema Linux
#define SPRITES_DIR "/home/vitim/Desktop/Geral/Pastas/Spritesjogo/"

enum {
    CIMA,
    BAIXO,
    DIREITA,
    ESQUERDA
};

typedef struct {
    int posx;
    int posy;
    ALLEGRO_BITMAP* personagem;
    int colisao;
} Vasco;

typedef struct {
    int movimento;
    int posx;
    int posy;
    bool ativa;
}Bala;

// Função para checar a inicialização (retorna 99 em caso de falha para debug)
int teste(bool resultado_init, const char* mensagem) {
    if (!resultado_init) {
        al_show_native_message_box(NULL, "ERRO", "AVISO:", mensagem, NULL, ALLEGRO_MESSAGEBOX_ERROR);
        return 99; // Retorna 99, evitando o confuso -1/255
    }
    return 0;
}

int main() {
    bool fim = false;
    bool teclas[] = { false, false, false, false }; 

    ALLEGRO_DISPLAY* display = NULL;
    ALLEGRO_TIMER* timer = NULL;
    ALLEGRO_FONT* font = NULL;
    ALLEGRO_EVENT_QUEUE* fila_eventos = NULL;

    Bala bala[MAX_BALAS];
    int cont = 0; 

    Vasco canga;
    canga.posx = LARGURA / 2;
    canga.posy = ALTURA / 2;
    int velocidade_personagem = 3;
    int velocidade_bala = 10;
    int fps = 60;
    int retorno_erro = 0;

    // --- INICIALIZAÇÕES DO ALLEGRO ---
    if ((retorno_erro = teste(al_init(), "Falha ao inicializar o Allegro")) != 0) return retorno_erro;

    // Instalações
    if ((retorno_erro = teste(al_install_keyboard(), "Falha ao instalar o teclado")) != 0) return retorno_erro;
    al_init_font_addon();
    al_init_ttf_addon();
    al_init_primitives_addon();
    if ((retorno_erro = teste(al_init_image_addon(), "Falha ao inicializar o addon de imagem")) != 0) return retorno_erro;

    display = al_create_display(LARGURA, ALTURA);
    if ((retorno_erro = teste(display, "Falha ao criar o display")) != 0) return retorno_erro;

    timer = al_create_timer(1.0 / fps);
    if ((retorno_erro = teste(timer, "Falha ao criar o timer")) != 0) return retorno_erro;
    
    // --- CARREGAMENTO DE RECURSOS ---
    
    char path_personagem[256];
    char path_background[256];
    char path_font[256];
    
    // Constrói os caminhos completos
    snprintf(path_personagem, sizeof(path_personagem), "%s%s", SPRITES_DIR, "Sprite-0002.png");
    snprintf(path_background, sizeof(path_background), "%s%s", SPRITES_DIR, "backgroud.png");
    snprintf(path_font, sizeof(path_font), "%s%s", SPRITES_DIR, "FiftiesMovies.ttf");


    canga.personagem = al_load_bitmap(path_personagem);
    if ((retorno_erro = teste(canga.personagem, "Falha ao carregar a imagem do personagem. Verifique o caminho e as dependências PNG.")) != 0) return retorno_erro;

    ALLEGRO_BITMAP* background = al_load_bitmap(path_background);
    if ((retorno_erro = teste(background, "Falha ao carregar a imagem de fundo.")) != 0) return retorno_erro; 

    // Flags corrigidas (0 em vez de NULL)
    font = al_load_font(path_font, 20, 0); 
    if ((retorno_erro = teste(font, "Falha ao carregar a fonte. Verifique o caminho e se o Allegro TTF está instalado.")) != 0) return retorno_erro;

    fila_eventos = al_create_event_queue();
    if ((retorno_erro = teste(fila_eventos, "Falha ao criar a fila de eventos")) != 0) return retorno_erro;

    // Registro dos Eventos
    al_register_event_source(fila_eventos, al_get_keyboard_event_source());
    al_register_event_source(fila_eventos, al_get_display_event_source(display));
    al_register_event_source(fila_eventos, al_get_timer_event_source(timer));

    al_clear_to_color(al_map_rgb(0, 0, 0));
    al_start_timer(timer);

    // --- LOOP PRINCIPAL DO JOGO ---
    while (!fim) {
        ALLEGRO_EVENT ev;
        al_wait_for_event(fila_eventos, &ev);

        // Processamento de Teclas DOWN
        if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
            switch (ev.keyboard.keycode) {
            case ALLEGRO_KEY_W:
                teclas[CIMA] = true; break;
            case ALLEGRO_KEY_S:
                teclas[BAIXO] = true; break;
            case ALLEGRO_KEY_D:
                teclas[DIREITA] = true; break;
            case ALLEGRO_KEY_A:
                teclas[ESQUERDA] = true; break;
            default:
                break;
            }
        }
        // Processamento de Teclas UP (Disparo)
        else if (ev.type == ALLEGRO_EVENT_KEY_UP) {
            switch (ev.keyboard.keycode) {
            case ALLEGRO_KEY_W:
                teclas[CIMA] = false; break;
            case ALLEGRO_KEY_S:
                teclas[BAIXO] = false; break;
            case ALLEGRO_KEY_D:
                teclas[DIREITA] = false; break;
            case ALLEGRO_KEY_A:
                teclas[ESQUERDA] = false; break;

            case ALLEGRO_KEY_UP:
            case ALLEGRO_KEY_DOWN:
            case ALLEGRO_KEY_LEFT:
            case ALLEGRO_KEY_RIGHT:
                if (cont < MAX_BALAS) {
                    Bala* nova_bala = &bala[cont];

                    if (ev.keyboard.keycode == ALLEGRO_KEY_UP) {
                        nova_bala->movimento = CIMA;
                    } else if (ev.keyboard.keycode == ALLEGRO_KEY_DOWN) {
                        nova_bala->movimento = BAIXO;
                    } else if (ev.keyboard.keycode == ALLEGRO_KEY_LEFT) {
                        nova_bala->movimento = ESQUERDA;
                    } else if (ev.keyboard.keycode == ALLEGRO_KEY_RIGHT) {
                        nova_bala->movimento = DIREITA;
                    }
                    
                    nova_bala->posx = canga.posx + 30; 
                    nova_bala->posy = canga.posy + 50;
                    nova_bala->ativa = true; 
                    
                    cont++;
                }
                break;
            default:
                break;
            }
        }
        
        // Lógica de Atualização (Física e Movimento)
        else if (ev.type == ALLEGRO_EVENT_TIMER) {
            // Movimento do Personagem
            if (teclas[CIMA]) canga.posy -= velocidade_personagem;
            if (teclas[BAIXO]) canga.posy += velocidade_personagem;
            if (teclas[DIREITA]) canga.posx += velocidade_personagem;
            if (teclas[ESQUERDA]) canga.posx -= velocidade_personagem;

            // Movimento das Balas
            for (int i = 0; i < cont; i++) {
                if (bala[i].ativa) {
                    if (bala[i].movimento == CIMA) {
                        bala[i].posy -= velocidade_bala;
                    } else if (bala[i].movimento == BAIXO) {
                        bala[i].posy += velocidade_bala;
                    } else if (bala[i].movimento == DIREITA) {
                        bala[i].posx += velocidade_bala;
                    } else if (bala[i].movimento == ESQUERDA) {
                        bala[i].posx -= velocidade_bala;
                    }
                    
                    // Desativa a bala se sair da tela
                    if (bala[i].posx < 0 || bala[i].posx > LARGURA ||
                        bala[i].posy < 0 || bala[i].posy > ALTURA) 
                    {
                        bala[i].ativa = false;
                    }
                }
            }

            // --- DESENHO (RENDERIZAÇÃO) ---
            al_clear_to_color(al_map_rgb(0, 0, 0)); 
            
            if(background)
                al_draw_bitmap(background, 0, 0, 0); 
            
            al_draw_bitmap(canga.personagem, canga.posx, canga.posy, 0);
            
            al_draw_rectangle(canga.posx+10, canga.posy+10, canga.posx + 55, canga.posy + 55, al_map_rgb(255, 0, 0), 3);
            
            // Desenha apenas as balas ATIVAS
            for (int i = 0; i < cont; i++) {
                if (bala[i].ativa) {
                    al_draw_filled_circle(bala[i].posx, bala[i].posy, 5, al_map_rgb(255, 255, 0));
                }
            }
            
            al_flip_display();
        }

        if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            fim = true;
        }
    }

    // --- LIMPEZA ---
    if (canga.personagem) al_destroy_bitmap(canga.personagem);
    if (background) al_destroy_bitmap(background);
    
    if (display) al_destroy_display(display);
    if (timer) al_destroy_timer(timer);
    if (font) al_destroy_font(font);
    if (fila_eventos) al_destroy_event_queue(fila_eventos);

    return 0;
}