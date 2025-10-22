ARQUIVO_MAIN := ./codigo/main.c

SAIDA := jogo.out
ALLEGRO := \
	-lallegro \
	-lallegro_main \
	-lallegro_audio \
	-lallegro_dialog \
	-lallegro_ttf \
	-lallegro_image \
 	-lallegro_color \
	-lallegro_memfile \
	-lallegro_acodec \
	-lallegro_primitives \
	-lallegro_font
FLAGS := ${ALLEGRO} -g -Wall


linux: ${ARQUIVO_MAIN}
	gcc ${ARQUIVO_MAIN} ${FLAGS} -c -o main.o
	gcc main.o -o ${SAIDA} ${FLAGS}

	@rm *.o
	@./${SAIDA}
