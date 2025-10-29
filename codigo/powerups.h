#include "jogador.h"
#include <allegro5/allegro_font.h>

typedef enum {
    AUMENTO_DANO,
    AUMENTO_VDA,
    AUMENTO_VDM,
} EPowerUps;

void desenhar_powerups(EPowerUps powers[3], ALLEGRO_FONT *fonte);

void aplicar_power(Jogador *canga, EPowerUps power);
