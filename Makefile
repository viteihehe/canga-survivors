CODIGOS := ${shell find -name *.c}

SAIDA := cangasurvs.out
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


dev: ${CODIGOS}
	gcc ${CODIGOS} ${ALLEGRO} -lm -g -Wall -o ${SAIDA}

	@./${SAIDA}
	@rm ${SAIDA}


tar: ${CODIGOS}
	gcc ${CODIGOS} ${ALLEGRO} -o ${SAIDA}
	tar -czf CangasLinux.tar.gz \
		--exclude ./materiais/sprites/desuso \
		--exclude ./materiais/repositorio \
		./materiais \
		${SAIDA}

	@rm ${SAIDA}
