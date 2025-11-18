#include "../constantes.h"
#include "../utils.h"
#include "quicksort.h"
#include <allegro5/allegro_primitives.h>
#include <stdio.h>

void desenhar_pontuacao(int pontos, ALLEGRO_FONT *fonte) {
    char pontuacao[30];
    sprintf(pontuacao, "Pontos: %d", pontos);
    desenhar_caixa_texto(pontuacao, COR_BRANCO, 850, 30, 200, 40, fonte);
}

void tela_morte(
    int pontos,
    ALLEGRO_FONT *fonte_titulo,
    ALLEGRO_FONT *fonte,
    char sigla[],
    char *letra,
    int *op,
    bool *selecionou
) {
    al_clear_to_color(COR_PRETO);
    

    desenhar_caixa_texto(
        "PONTUAÇÕES", COR_BRANCO, 500, 70, 580, 80, fonte_titulo
    );
    al_draw_rectangle(180, -10, 820, 800, COR_BRANCO, 10);

    al_draw_rectangle(180, 660, 820, 800, COR_BRANCO, 10);
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

void exibir_lista(ALLEGRO_FONT* fonte, ALLEGRO_FONT* fonte_titulo) {
    
    

    desenhar_caixa_texto(
        "PONTUAÇÕES", COR_BRANCO, 500, 70, 580, 80, fonte_titulo
    );
    al_draw_rectangle(180, -10, 820, 800, COR_BRANCO, 10);

    al_draw_rectangle(180, 660, 820, 800, COR_BRANCO, 10);

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
            Usuarios *usuarios =
                (Usuarios *)malloc(quantidade * sizeof(Usuarios));

            fread(usuarios, sizeof(Usuarios), quantidade, file);
            fclose(file);

            quicksort(0, quantidade - 1, usuarios);

            int posy_inicial = 200;
            for (int i = 0; i < quantidade; i++) {
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
                posy_inicial += 80;
            }

            free(usuarios);
        }
}