#include "../constantes.h"
#include "../utils.h"
#include "quicksort.h"
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_primitives.h>
#include <stdio.h>

void desenhar_pontuacao(int pontos, ALLEGRO_FONT *fonte) {
    char pontuacao[30];
    sprintf(pontuacao, "Pontos: %d", pontos);
    desenhar_caixa_texto(pontuacao, COR_BRANCO, 850, 30, 200, 40, fonte);
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
    ALLEGRO_FONT *fonte, int *op, bool *selecionou, char sigla[], char *letra
) {
    al_draw_text(
        fonte, COR_BRANCO, 350, 700, ALLEGRO_ALIGN_CENTRE, "insira sua sigla:"
    );
    switch (*op) {
    case 0:
        al_draw_textf(
            fonte, COR_BRANCO, 520, 700, ALLEGRO_ALIGN_CENTRE, "%c _ _", *letra
        );
        break;
    case 1:
        al_draw_textf(
            fonte,
            COR_BRANCO,
            520,
            700,
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
            520,
            700,
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
    ALLEGRO_FONT *fonte_titulo,
    ALLEGRO_FONT *fonte,
    char sigla[],
    char *letra,
    int *op,
    bool *selecionou,
    ALLEGRO_SAMPLE *escolha,
    ALLEGRO_SAMPLE *selecao
) {
    al_draw_filled_rectangle(170, -10, 830, 800, COR_PRETO);
    desenhar_caixa_texto(
        "PONTUAÇÕES", COR_BRANCO, 500, 70, 580, 80, fonte_titulo
    );
    al_draw_rectangle(170, -10, 830, 800, COR_BRANCO, 10);

    al_draw_rectangle(170, 660, 830, 800, COR_BRANCO, 10);
    exibir_escrita_sigla(fonte, op, selecionou, sigla, letra);
    selecao_letra(evento, letra, selecionou, selecao, escolha);
}

void salvar_arquivo(int pontos, char sigla[]) {
    char dir[50];
    sprintf(dir, "./materiais/repositorio/pontos.bin");
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

void exibir_lista(ALLEGRO_FONT *fonte, ALLEGRO_FONT *fonte_titulo) {

    al_draw_filled_rectangle(170, -10, 830, 800, COR_PRETO);
    desenhar_caixa_texto(
        "PONTUAÇÕES", COR_BRANCO, 500, 70, 580, 80, fonte_titulo
    );
    al_draw_rectangle(170, -10, 830, 800, COR_BRANCO, 10);

    al_draw_rectangle(170, 660, 830, 800, COR_BRANCO, 10);

    char dir[50];
    sprintf(dir, "./materiais/repositorio/pontos.bin");
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
        Usuarios *usuarios = (Usuarios *)malloc(quantidade * sizeof(Usuarios));

        fread(usuarios, sizeof(Usuarios), quantidade, file);
        fclose(file);

        quicksort(0, quantidade - 1, usuarios);

        int posy_inicial = 200;
        for (int i = 0; i < quantidade && quantidade <= 5; i++) {
            if (i == 0) {
                al_draw_text(
                    fonte,
                    COR_BRANCO,
                    LARGURA / 2 - 300,
                    posy_inicial - 40,
                    0,
                    "-------------------------------------------------"
                );
                al_draw_textf(
                    fonte,
                    COR_BRANCO,
                    LARGURA / 2,
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
                    LARGURA / 2 - 300,
                    posy_inicial + 40,
                    0,
                    "-------------------------------------------------"
                );
            } else {
                al_draw_textf(
                    fonte,
                    COR_BRANCO,
                    LARGURA / 2,
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
                    LARGURA / 2 - 300,
                    posy_inicial + 40,
                    0,
                    "-------------------------------------------------"
                );
            }
            posy_inicial += 80;
        }

        free(usuarios);
    }
}

Usuarios * retorna_score(char sigla[]) {
    char dir[50];
    sprintf(dir, "./materiais/repositorio/pontos.bin");
    FILE *file = fopen(dir, "rb");
    if(file == NULL) {
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
    al_draw_filled_rectangle(170, -10, 830, 800, COR_PRETO);
    al_draw_rectangle(170, -10, 830, 800, COR_BRANCO, 10);

    al_draw_rectangle(170, 660, 830, 800, COR_BRANCO, 10);

    if (*op < 3) {
        exibir_lista(fonte, fonte_titulo);

        al_draw_rectangle(
            170, ALTURA / 2 + 190, 830, ALTURA / 2 + 280, COR_BRANCO, 10
        );
        al_draw_text(
            fonte,
            COR_BRANCO,
            LARGURA / 2,
            ALTURA / 2 + 220,
            ALLEGRO_ALIGN_CENTRE,
            "Insira a sigla que deseja buscar:"
        );
        exibir_escrita_sigla(fonte, op, selecionou, sigla_busca, letra);
        selecao_letra(evento, letra, selecionou, selecao, escolha);
    }

    if (*op >= 3) {
        al_draw_filled_rectangle(170, -10, 830, 800, COR_PRETO);
        desenhar_caixa_texto(
            "PONTUAÇÕES", COR_BRANCO, 500, 70, 580, 80, fonte_titulo
        );
        al_draw_rectangle(170, -10, 830, 800, COR_BRANCO, 10);

        al_draw_rectangle(170, 660, 830, 800, COR_BRANCO, 10);
        Usuarios *atual = retorna_score(sigla_busca);
        if (atual == NULL) {
            al_draw_text(
                fonte,
                COR_BRANCO,
                LARGURA / 2-250,
                ALTURA / 2,
                0,
                "Não foi possivel encontrar o score"
            );
            al_draw_text(
                    fonte,
                    COR_BRANCO,
                    LARGURA/2,
                    700,
                    ALLEGRO_ALIGN_CENTRE,
                    "Aperte [esc] para voltar"
                );
        } else {
            al_draw_textf(fonte, COR_BRANCO, LARGURA / 2, ALTURA / 2-80, ALLEGRO_ALIGN_CENTER, "Pontuação mais alta:\n");  
            al_draw_textf(fonte, COR_BRANCO, LARGURA / 2, ALTURA / 2, ALLEGRO_ALIGN_CENTER, "%s, %d\n", atual->sigla, atual->pontos);
            al_draw_text(
                    fonte,
                    COR_BRANCO,
                    LARGURA/2,
                    700,
                    ALLEGRO_ALIGN_CENTRE,
                    "Aperte [esc] para voltar"
                );
        }
        if(atual != NULL) {
            free(atual);
        }
    }
}


