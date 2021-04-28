#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <math.h>
#include <allegro5/allegro_primitives.h>

const float FPS = 60;
const int SCREEN_W = 640*2;
const int SCREEN_H = 480;


int main(int argc, char **argv){

   ALLEGRO_DISPLAY *display = NULL;
   ALLEGRO_EVENT_QUEUE *event_queue = NULL;

   bool redraw = true;

   //------------------------------ procedimentos de inicializacao ----------------------------
   if(!al_init()) {
      fprintf(stderr, "failed to initialize allegro!\n");
      return -1;
   }

   display = al_create_display(SCREEN_W, SCREEN_H);
   if(!display) {
      fprintf(stderr, "failed to create display!\n");
      return -1;
   }

    al_install_keyboard();
    al_init_primitives_addon();

   // ----------------- cria a fila de eventos ----------------------------------------

   event_queue = al_create_event_queue();
   if(!event_queue) {
      fprintf(stderr, "failed to create event_queue!\n");
      al_destroy_display(display);
      return -1;
   }
   //registra que a fila de eventos deve detectar quando uma tecla for pressionada no teclado
   al_register_event_source(event_queue, al_get_keyboard_event_source());

   // ------------------------ modificando os valores de entrada ----------------------------------

    //variavel do tipo char[] que recebe um texto
    char my_text[20];
	//variavel que recebe o valor de entrada 
    int input_value = 100;
	//posicao x da barra de entrada
    int x_bar = 50;

	//inicializa o modulo allegro que carrega as fontes
    al_init_font_addon();
	//inicializa o modulo allegro que entende arquivos tff de fontes
    al_init_ttf_addon();

	//carrega o arquivo arial.ttf da fonte Arial e define que sera usado o tamanho 32 (segundo parametro)
    ALLEGRO_FONT *size_32 = al_load_font("arial.ttf", 32, 1);

	//variavel que informa se o programa deve terminar ou nao
    bool quit = false;
	//enquanto nao for pra terminar, faca:
    while(!quit) {

		//cria um texto "Minha entrada: input_value" e o coloca na variavel my_text
        sprintf(my_text, "Minha entrada: %d", input_value);
		//colore toda a tela de preto
        al_clear_to_color(al_map_rgb(0,0,0));
        //imprime o texto armazenado em my_text na posicao x=10,y=10 e com a cor rgb(128,200,30)
        al_draw_text(size_32, al_map_rgb(128, 200, 30), 10, 10, 0, my_text);
        //desenha um retangulo de cor rgb(100,0,0) nas posicoes x1=10, y1=80 (vertice superior esquerdo) x2=x_bar, y2=120 (vertice inferior direito)
        al_draw_filled_rectangle(10, 80, x_bar, 120, al_map_rgb(100,0,0));
        //imprime um texto informativo na tela
        al_draw_text(size_32, al_map_rgb(128, 200, 200), 10, 400, 0, "aperte as teclas A, S, D, W e ENTER para finalizar");
		//reinicializa a tela
        al_flip_display();
		
		//declara uma variavel do tipo evento
        ALLEGRO_EVENT e;
		//espera por um evento externo (tecla pressionada) e armazena o evento na variavel e
        al_wait_for_event(event_queue, &e);

		//se o tipo do evento for uma tecla pressionada
        if(e.type == ALLEGRO_EVENT_KEY_DOWN) {
			//verifica qual tecla foi
            switch(e.keyboard.keycode) {
				//se a tecla for o W
                case ALLEGRO_KEY_W:
                    input_value += 5;
                break;

				//se a tecla for o S
                case ALLEGRO_KEY_S:
                    input_value -= 5;
                break;

                case ALLEGRO_KEY_A:
                    x_bar -= 5;
                break;

                case ALLEGRO_KEY_D:
                    x_bar += 5;
                break;

                case ALLEGRO_KEY_ENTER:
                    quit = true;
                break;
                }

            }
	//se nao apertei ENTER, volta para o inicio do while e refaz tudo
    }

   // -------------------- terminando o jogo ------------------------------------------

   al_destroy_display(display);
   al_destroy_event_queue(event_queue);

   return 0;
}
