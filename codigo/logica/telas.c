#include "telas.h"
#include "../constantes.h"
#include "../utils.h"
#include "quicksort.h"
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/bitmap.h>
#include <allegro5/bitmap_draw.h>
#include <allegro5/bitmap_io.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void desenhar_pontuacao(int pontos, ALLEGRO_FONT *fonte) {
    char pontuacao[30];
    sprintf(pontuacao, "%d", pontos);
    desenhar_caixa_texto(pontuacao, COR_BRANCO, 869, 35, 160, 50, fonte);
}

void selecao_letra(
    ALLEGRO_EVENT evento,
    char *letra,
    bool *selecionou,
    ALLEGRO_SAMPLE *selecao,
    ALLEGRO_SAMPLE *escolha
) {
    if (evento.type == ALLEGRO_EVENT_KEY_UP) {
        if (evento.keyboard.keycode == ALLEGRO_KEY_S ||
            evento.keyboard.keycode == ALLEGRO_KEY_DOWN) {
            if (*letra > '@' && *letra < '[') {
                *letra += 1;
            } else {
                *letra = 'A';
            }
            al_play_sample(selecao, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
        }
        if (evento.keyboard.keycode == ALLEGRO_KEY_W ||
            evento.keyboard.keycode == ALLEGRO_KEY_UP) {
            if (*letra > '@' && *letra < '[') {
                *letra -= 1;
            } else {
                *letra = 'Z';
            }
            al_play_sample(selecao, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
        }

        if ((evento.keyboard.keycode) == ALLEGRO_KEY_ENTER) {
            *selecionou = true;
        }
    }
}

void exibir_escrita_sigla(
    ALLEGRO_FONT *fonte,
    int *op,
    bool *selecionou,
    char sigla[],
    char *letra,
    int x,
    int y,
    char texto[]
) {

    al_draw_textf(
        fonte, COR_BRANCO, x - 150, y, ALLEGRO_ALIGN_CENTRE, "%s", texto
    );

    switch (*op) {
    case 0:
        al_draw_textf(
            fonte, COR_BRANCO, x, y, ALLEGRO_ALIGN_CENTRE, "%c _ _", *letra
        );
        break;
    case 1:
        al_draw_textf(
            fonte,
            COR_BRANCO,
            x,
            y,
            ALLEGRO_ALIGN_CENTRE,
            "%c %c _",
            sigla[0],
            *letra
        );
        break;
    case 2:
        al_draw_textf(
            fonte,
            COR_BRANCO,
            x,
            y,
            ALLEGRO_ALIGN_CENTRE,
            "%c %c %c",
            sigla[0],
            sigla[1],
            *letra
        );
        break;
    default:
        break;
    }
    if (*selecionou) {
        if (*op < 3) {
            sigla[*op] = *letra;
            (*op)++;
        }
        if (*op == 3) {
            sigla[3] = '\0';
        }
        *selecionou = false;
    }
}

void tela_morte(
    ALLEGRO_EVENT evento,
    int pontos,
    ALLEGRO_FONT *fonte_menor,
    ALLEGRO_FONT *fonte,
    ALLEGRO_FONT *fonte_frase,
    char sigla[],
    char *letra,
    int *op,
    bool *selecionou,
    ALLEGRO_SAMPLE *escolha,
    ALLEGRO_SAMPLE *selecao,
    ALLEGRO_BITMAP *caveira,
    Estatisticas est

) {
    srand(time(NULL));
    int escreveu = 0;
    static int frase_escolhida = -1;

    al_clear_to_color(al_map_rgb(152, 61, 58));

    al_draw_filled_rectangle(0, 50, 960, 150, al_map_rgb(122, 35, 35));
    al_draw_filled_ellipse(100, 100, 100, 100, al_map_rgb(103, 30, 30));
    al_draw_scaled_bitmap(caveira, 0, 0, 64, 64, -28, -5, 260, 260, 0);
    al_draw_filled_rectangle(50, 250, 395, 600, al_map_rgb(122, 35, 35));
    al_draw_rectangle(45, 245, 395, 595, COR_BRANCO, 5);
    al_draw_text(fonte, COR_BRANCO, 120, 260, 0, "Estatisticas:");
    int x = 310;
    al_draw_textf(
        fonte, COR_BRANCO, 70, x, 0, "Pontuação: %d", est.pontuacao_run
    );
    al_draw_textf(
        fonte, COR_BRANCO, 70, x + 50, 0, "Passos dados: %d", est.passos_dados
    );
    al_draw_textf(
        fonte,
        COR_BRANCO,
        70,
        x + 100,
        0,
        "Nivel atingido: %d",
        est.nivel_atingido
    );
    al_draw_textf(
        fonte, COR_BRANCO, 70, x + 150, 0, "Dano causado: %d", est.Dano_causado
    );
    al_draw_textf(
        fonte, COR_BRANCO, 70, x + 200, 0, "Dano sofrido: %d", est.Dano_sofrido
    );
    al_draw_textf(
        fonte,
        COR_BRANCO,
        70,
        x + 250,
        0,
        "Total de abates: %d",
        est.total_inimigos_mortos
    );

    if (!escreveu) {
        if (frase_escolhida == -1) {
            frase_escolhida = (rand() % 4) + 1;
        }
        if (!escreveu) {
            switch (frase_escolhida) {
            case 1:
                al_draw_text(
                    fonte_frase,
                    COR_BRANCO,
                    250,
                    80,
                    0,
                    "Ei rapaz deixe de safadeza"
                );
                escreveu = 1;
                break;
            case 2:
                al_draw_text(
                    fonte_frase, COR_BRANCO, 250, 80, 0, "SE LASCOU HAHAHAHAHA"
                );
                escreveu = 1;
                break;
            case 3:
                al_draw_text(
                    fonte_frase, COR_BRANCO, 250, 80, 0, "GET GOOD!!!"
                );
                escreveu = 1;
                break;
            case 4:
                al_draw_text(
                    fonte_frase,
                    COR_BRANCO,
                    250,
                    80,
                    0,
                    "Infelizmente aconteceu :("
                );
                escreveu = 1;
                break;
            default:
                break;
            }
        }
        escreveu = 1;
    }

    desenhar_caixa_texto(
        "Aperte [espaço] para recomeçar",
        COR_BRANCO,
        LARGURA / 2,
        700,
        500,
        80,
        fonte
    );

    exibir_lista(fonte, fonte_frase, 500, 250, 845, 620, 0);
    exibir_escrita_sigla(
        fonte_menor, op, selecionou, sigla, letra, 760, 580, "Insira sua sigla:"
    );
    selecao_letra(evento, letra, selecionou, selecao, escolha);
}

void salvar_arquivo(int pontos, char sigla[]) {
    char dir[50];
    sprintf(dir, "./saves/pontuacoes.bin");
    FILE *file;

    bool gravado = false;
    if (!gravado) {
        file = fopen(dir, "ab");
        Usuarios atual;
        strcpy(atual.sigla, sigla);
        atual.pontos = pontos;
        fwrite(&atual, sizeof(Usuarios), 1, file);
        fclose(file);
        gravado = true;
    }
}

void exibir_lista(
    ALLEGRO_FONT *fonte,
    ALLEGRO_FONT *fonte_titulo,
    int x1,
    int y1,
    int x2,
    int y2,
    int tipo
) {
    if (!tipo) {
        al_draw_filled_rectangle(x1, y1, x2, y2, COR_PRETO);
        al_draw_text(fonte, COR_BRANCO, x1 + 130, y1 + 25, 0, "Rank:");
        al_draw_rectangle(x1, y1, x2, y2, COR_BRANCO, 10);

        char dir[50];
        sprintf(dir, "./saves/pontuacoes.bin");
        FILE *file;
        file = fopen(dir, "rb");
        if (file == NULL) {
            file = fopen(dir, "ab");
            fclose(file);
            file = fopen(dir, "rb");
        }

        fseek(file, 0, SEEK_END);
        long tam = ftell(file);
        fseek(file, 0, SEEK_SET);
        int quantidade = tam / sizeof(Usuarios);

        if (quantidade != 0) {
            Usuarios *usuarios =
                (Usuarios *)malloc(quantidade * sizeof(Usuarios));

            fread(usuarios, sizeof(Usuarios), quantidade, file);
            fclose(file);

            quicksort(0, quantidade - 1, usuarios);

            int posx = (x1);
            int posy_inicial = y1 + 100;
            for (int i = 0; i < quantidade && quantidade <= 3; i++) {
                if (i == 0) {
                    al_draw_text(
                        fonte,
                        COR_BRANCO,
                        posx,
                        posy_inicial - 40,
                        0,
                        "--------------------------"
                    );
                    al_draw_textf(
                        fonte,
                        COR_BRANCO,
                        posx + 180,
                        posy_inicial,
                        ALLEGRO_ALIGN_CENTER,
                        "%dº %s %d pontos",
                        i + 1,
                        usuarios[i].sigla,
                        usuarios[i].pontos
                    );
                    al_draw_text(
                        fonte,
                        COR_BRANCO,
                        posx,
                        posy_inicial + 40,
                        0,
                        "--------------------------"
                    );
                } else {
                    al_draw_textf(
                        fonte,
                        COR_BRANCO,
                        posx + 180,
                        posy_inicial,
                        ALLEGRO_ALIGN_CENTER,
                        "%dº %s %d pontos",
                        i + 1,
                        usuarios[i].sigla,
                        usuarios[i].pontos
                    );
                    al_draw_text(
                        fonte,
                        COR_BRANCO,
                        posx,
                        posy_inicial + 40,
                        0,
                        "--------------------------"
                    );
                }
                posy_inicial += 80;
            }

            free(usuarios);
        }

    } else {
        al_draw_filled_rectangle(x1, y1, x2, y2, COR_PRETO);
        al_draw_text(fonte, COR_BRANCO, x1 + 300, y1 + 25, 0, "Rank:");
        al_draw_rectangle(x1, y1, x2, y2, COR_BRANCO, 10);

        char dir[50];
        sprintf(dir, "./saves/pontuacoes.bin");
        FILE *file;
        file = fopen(dir, "rb");
        if (file == NULL) {
            file = fopen(dir, "ab");
            fclose(file);
            file = fopen(dir, "rb");
        }

        fseek(file, 0, SEEK_END);
        long tam = ftell(file);
        fseek(file, 0, SEEK_SET);
        int quantidade = tam / sizeof(Usuarios);

        if (quantidade != 0) {
            Usuarios *usuarios =
                (Usuarios *)malloc(quantidade * sizeof(Usuarios));

            fread(usuarios, sizeof(Usuarios), quantidade, file);
            fclose(file);

            quicksort(0, quantidade - 1, usuarios);

            int posx = x1 + 150;
            int posy_inicial = y1 + 100;
            for (int i = 0; i < quantidade && quantidade <= 6; i++) {
                if (i == 0) {
                    al_draw_text(
                        fonte,
                        COR_BRANCO,
                        posx - 150,
                        posy_inicial - 40,
                        0,
                        "---------------------------------------------------"
                    );
                    al_draw_textf(
                        fonte,
                        COR_BRANCO,
                        posx + 180,
                        posy_inicial,
                        ALLEGRO_ALIGN_CENTER,
                        "%dº %s %d pontos",
                        i + 1,
                        usuarios[i].sigla,
                        usuarios[i].pontos
                    );
                    al_draw_text(
                        fonte,
                        COR_BRANCO,
                        posx - 150,
                        posy_inicial + 40,
                        0,
                        "---------------------------------------------------"
                    );
                } else {
                    al_draw_textf(
                        fonte,
                        COR_BRANCO,
                        posx + 180,
                        posy_inicial,
                        ALLEGRO_ALIGN_CENTER,
                        "%dº %s %d pontos",
                        i + 1,
                        usuarios[i].sigla,
                        usuarios[i].pontos
                    );
                    al_draw_text(
                        fonte,
                        COR_BRANCO,
                        posx - 150,
                        posy_inicial + 40,
                        0,
                        "---------------------------------------------------"
                    );
                }
                posy_inicial += 80;
            }

            free(usuarios);
        }
    }
}

Usuarios *retorna_score(char sigla[]) {
    char dir[50];
    sprintf(dir, "./saves/pontuacoes.bin");
    FILE *file = fopen(dir, "rb");
    if (file == NULL) {
        return NULL;
    }
    Usuarios *retorno = (Usuarios *)malloc(sizeof(Usuarios));
    while (fread(retorno, sizeof(Usuarios), 1, file)) {
        if (strcmp(retorno->sigla, sigla) == 0) {
            fclose(file);
            return retorno;
        }
    }
    fclose(file);
    return NULL;
}

void busca_pontucao(
    ALLEGRO_EVENT evento,
    ALLEGRO_FONT *fonte,
    ALLEGRO_FONT *fonte_titulo,
    int *op,
    bool *selecionou,
    char *letra,
    ALLEGRO_SAMPLE *escolha,
    ALLEGRO_SAMPLE *selecao,
    char sigla_busca[]
) {
    // al_draw_filled_rectangle(170, -10, 830, 800, COR_PRETO);
    // al_draw_rectangle(170, -10, 830, 800, COR_BRANCO, 10);

    // al_draw_rectangle(170, 660, 830, 800, COR_BRANCO, 10);

    exibir_lista(fonte, fonte_titulo, 170, 0, 830, 800, 1);

    if (*op < 3) {
        // exibir_lista(fonte, fonte_titulo);

        al_draw_rectangle(
            170, ALTURA / 2.0 + 190, 830, ALTURA / 2.0 + 280, COR_BRANCO, 10
        );
        al_draw_text(
            fonte,
            COR_BRANCO,
            LARGURA / 2.0 - 50,
            ALTURA / 2.0 + 220,
            ALLEGRO_ALIGN_CENTRE,
            "Insira a sigla que deseja buscar:"
        );
        exibir_escrita_sigla(
            fonte,
            op,
            selecionou,
            sigla_busca,
            letra,
            LARGURA / 2.0 + 250,
            ALTURA / 2.0 + 220,
            " "
        );
        selecao_letra(evento, letra, selecionou, selecao, escolha);
    }

    if (*op >= 3) {

        al_draw_filled_rectangle(175, 50, 825, 600, COR_PRETO);
        al_draw_rectangle(170, 660, 830, 800, COR_BRANCO, 10);
        Usuarios *atual = retorna_score(sigla_busca);
        if (atual == NULL) {
            al_draw_text(
                fonte,
                COR_BRANCO,
                LARGURA / 2.0 - 250,
                ALTURA / 2.0,
                0,
                "Não foi possivel encontrar o score"
            );
            al_draw_text(
                fonte,
                COR_BRANCO,
                LARGURA / 2.0,
                700,
                ALLEGRO_ALIGN_CENTRE,
                "Aperte [esc] para voltar"
            );
        } else {
            al_draw_textf(
                fonte,
                COR_BRANCO,
                LARGURA / 2.0,
                ALTURA / 2.0 - 80,
                ALLEGRO_ALIGN_CENTER,
                "Pontuação mais alta:\n"
            );
            al_draw_textf(
                fonte,
                COR_BRANCO,
                LARGURA / 2.0,
                ALTURA / 2.0,
                ALLEGRO_ALIGN_CENTER,
                "%s, %d\n",
                atual->sigla,
                atual->pontos
            );
            al_draw_text(
                fonte,
                COR_BRANCO,
                LARGURA / 2.0,
                700,
                ALLEGRO_ALIGN_CENTRE,
                "Aperte [esc] para voltar"
            );
        }
        if (atual != NULL) {
            free(atual);
        }
    }
}

void desenhar_guias(FolhaSprites sprites, ALLEGRO_FONT *fonte) {
    int larg = 200;
    int altu = 150;

    // WASD
    desenhar_caixa_texto(
        "", COR_BRANCO, 0.25 * LARGURA, 0.75 * ALTURA, larg, altu, fonte
    );

    desenhar_caixa_texto(
        "Andar",
        COR_BRANCO,
        0.25 * LARGURA,
        0.75 * ALTURA - altu / 2.0 - 10,
        110,
        50,
        fonte
    );

    al_draw_scaled_bitmap(
        sprites.guia_wasd,
        0,
        0,
        64,
        64,
        0.25 * LARGURA - 100,
        0.75 * ALTURA - 85,
        200,
        200,
        0
    );

    // Setas
    desenhar_caixa_texto(
        "", COR_BRANCO, 0.75 * LARGURA, 0.75 * ALTURA, larg, altu, fonte
    );

    desenhar_caixa_texto(
        "Atirar",
        COR_BRANCO,
        0.75 * LARGURA,
        0.75 * ALTURA - altu / 2.0 - 10,
        110,
        50,
        fonte
    );

    al_draw_scaled_bitmap(
        sprites.guia_setas,
        0,
        0,
        64,
        64,
        0.75 * LARGURA - 100,
        0.75 * ALTURA - 85,
        200,
        200,
        0
    );
}