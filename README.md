# Canga Survivors
Canga Survivors é um jogo estilo rogue-like com temática nordestina, escrito utilizando a linguagem C em conjunto com a biblioteca Allegro5.

## Compilando
### Dependências
É necessário ter o GCC, o Make e toda a biblioteca Allegro5 instalados na sua máquina.

#### Fedora e derivados:
```bash
sudo dnf install gcc make allegro5-devel allegro5-addon-acodec-devel allegro5-addon-audio-devel allegro5-addon-dialog-devel allegro5-addon-image-devel allegro5-addon-physfs-devel allegro5-addon-ttf-devel allegro5-addon-video-devel
```

#### Ubuntu e derivados:
```bash
sudo apt install gcc make liballegro5-dev liballegro-acodec5-dev liballegro-audio5-dev liballegro-dialog5-dev liballegro-image5-dev liballegro-physfs5-dev liballegro-ttf5-dev liballegro-video5-dev
```

### Build
Execute o seguinte comando na mesma pasta que está o Makefile:

```bash
make
```

O jogo já é executado automaticamente após cada build, mas se quiser abrir sem compilar de novo basta rodar:

```
./jogo.out
```

## Bugs Conhecidos
- Em algumas situações o jogo pode sofrer input lag.
- Os inimigos podem atravessar ou ficarem presos no cenário.
- As balas do jogador são alocadas permanentemente na memória.

## Apêndice
![QueJogo](./materiais/repositorio/quejogo.jpeg)
