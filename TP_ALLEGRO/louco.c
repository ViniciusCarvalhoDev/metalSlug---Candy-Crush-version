#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>

//Programa simples para exibir uma imagem na tela por 3 segundos
//toda funcao que inicia com "al" eh uma funcao padrao do Allegro

int main (void)
{
//cria a variavel para receber a tela
ALLEGRO_DISPLAY *display = NULL;

//inicializa o Allegro
al_init();

//cria a tela com tamanho 400 x 350 (teste: altere os valores)
display = al_create_display(400, 350);

//inicializa o modulo que permite carregar imagens no jogo
al_init_image_addon();

//carrega a imagem louco.jpg e a atribui aa variavel image
ALLEGRO_BITMAP *image = al_load_bitmap("louco.jpg");

//coloca na tela a imagem armazenada na variavel image nas posicoes x=50, y=100
al_draw_bitmap(image, 50, 100, 0);

//atualiza a tela
al_flip_display();

//pausa a tela por 3.0 segundos
al_rest(3.0);

//destroi a variavel imagem = libera a memoria alocada para ela
al_destroy_bitmap(image);
//destroi a tela criada para o jogo
al_destroy_display(display);

return 0;
}
