#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <stdlib.h>
#include <time.h>

#define NUM_RUAS 10
 
const float FPS = 60;  
const int SCREEN_W = 640;
const int SCREEN_H = 480;
//variavel global que armazena o tamanho do bouncer (quadrado que vai navegar pela tela)
const int BOUNCER_SIZE = 32;
 
int main(int argc, char **argv){
   ALLEGRO_DISPLAY *display = NULL;
   ALLEGRO_EVENT_QUEUE *event_queue = NULL;
   ALLEGRO_TIMER *timer = NULL;
   ALLEGRO_BITMAP *bouncer = NULL;
   ALLEGRO_BITMAP *buses[NUM_RUAS];
   int playing = 1;
   int collision = 0;
   
   //posicoes x e y iniciais do frog
   float bouncer_x = SCREEN_W / 2.0 - BOUNCER_SIZE / 2.0;
   float bouncer_y = SCREEN_H - BOUNCER_SIZE;
   //o quanto as posicoes x e y vao variar ao longo do tempo. No t=1, se x do bouncer eh 40, no t=2, x do bouncer eh 40 + bouncer_dx = 36
   float bouncer_dx = SCREEN_W /20.0, bouncer_dy = (float)SCREEN_H / NUM_RUAS;
   
   float TAM_RUA = (float) SCREEN_H / NUM_RUAS;
   float LARGURA_BUS = TAM_RUA * 0.8;
   
   //automoveis
   float buses_x[NUM_RUAS];
   float buses_y[NUM_RUAS];
   float buses_comp[NUM_RUAS];
   float buses_dx[NUM_RUAS];
   
	int i;
	int j;

	

	//----------------------- rotinas de inicializacao ---------------------------------------
   if(!al_init()) {
      fprintf(stderr, "failed to initialize allegro!\n");
      return -1;
   }
 
   timer = al_create_timer(1.0 / FPS);
   if(!timer) {
      fprintf(stderr, "failed to create timer!\n");
      return -1;
   }
 
   display = al_create_display(SCREEN_W, SCREEN_H);
   if(!display) {
      fprintf(stderr, "failed to create display!\n");
      al_destroy_timer(timer);
      return -1;
   }
 
	//cria um bitmap quadrangular de tamanho BOUNCER_SIZE (variavel global declarada acima)
   bouncer = al_create_bitmap(BOUNCER_SIZE, BOUNCER_SIZE);
   if(!bouncer) {
      fprintf(stderr, "failed to create bouncer bitmap!\n");
      al_destroy_display(display);
      al_destroy_timer(timer);
      return -1;
   }
   
   //buses
   	for(i=0; i<NUM_RUAS; i++) {
		buses_y[i] = i*TAM_RUA + 0.1*TAM_RUA;
		buses_x[i] = 0;
		buses_comp[i] = rand()%(SCREEN_W/4) + 10;
		buses_dx[i] = 3*(float)rand()/(float)RAND_MAX;
		buses[i] = al_create_bitmap(buses_comp[i], LARGURA_BUS);
		al_set_target_bitmap(buses[i]);
		al_clear_to_color(al_map_rgb(rand()%256, rand()%256, rand()%256));
	}
   
   al_install_keyboard();
 
	//avisa o allegro que eu quero modificar as propriedades do bouncer
   al_set_target_bitmap(bouncer);
 
	//altera a cor do bouncer para rgb(255,0,255)
   al_clear_to_color(al_map_rgb(0, 255, 0));
 
	//avisa o allegro que agora eu quero modificar as propriedades da tela
   al_set_target_bitmap(al_get_backbuffer(display));
	//colore a tela de preto (rgb(0,0,0))
   al_clear_to_color(al_map_rgb(0,0,0));   
 
	//cria a fila de eventos
   event_queue = al_create_event_queue();
   if(!event_queue) {
      fprintf(stderr, "failed to create event_queue!\n");
      al_destroy_bitmap(bouncer);
      al_destroy_display(display);
      al_destroy_timer(timer);
      return -1;
   }
 
	//registra na fila de eventos que eu quero identificar quando a tela foi alterada
   al_register_event_source(event_queue, al_get_display_event_source(display));
	//registra na fila de eventos que eu quero identificar quando o tempo alterou de t para t+1
   al_register_event_source(event_queue, al_get_timer_event_source(timer));
   //registra que a fila de eventos deve detectar quando uma tecla for pressionada no teclado
   al_register_event_source(event_queue, al_get_keyboard_event_source());   
 

	//reinicializa a tela
   al_flip_display();
	//inicia o temporizador
   al_start_timer(timer);
 
	//enquanto playing for verdadeiro, faca:
   while(playing) {
      ALLEGRO_EVENT ev;
	  //espera por um evento e o armazena na variavel de evento ev
      al_wait_for_event(event_queue, &ev);
	  
	//se o tipo do evento for uma tecla pressionada
	if(ev.type == ALLEGRO_EVENT_KEY_DOWN) {
		//verifica qual tecla foi
		switch(ev.keyboard.keycode) {
			//se a tecla for o W
			case ALLEGRO_KEY_W:
				bouncer_y -= bouncer_dy;
				if(bouncer_y < 0) 
					playing = 0;					
			break;
			//se a tecla for o S
			case ALLEGRO_KEY_S:
				if(bouncer_y < SCREEN_H - BOUNCER_SIZE)			
				bouncer_y += bouncer_dy;
			break;

			case ALLEGRO_KEY_A:
				if(bouncer_x > 0) 
					bouncer_x -= bouncer_dx;
			break;

			case ALLEGRO_KEY_D:
				if(bouncer_x < SCREEN_W - BOUNCER_SIZE) 
					bouncer_x += bouncer_dx;
			break;

			case ALLEGRO_KEY_ESCAPE:
				playing = 0;
			break;
			}

		}	  
 
		//se o tipo de evento for um evento do temporizador, ou seja, se o tempo passou de t para t+1
    if(ev.type == ALLEGRO_EVENT_TIMER) {
		//limpo a tela
		 al_clear_to_color(al_map_rgb(0,0,0));
		//desenho o bouncer nas novas posicoes x e y
		 al_draw_bitmap(bouncer, bouncer_x, bouncer_y, 0);
		 
		for(i=0; i<NUM_RUAS; i++) {
			buses_x[i] += buses_dx[i];
			if(buses_x[i] > SCREEN_W)
				buses_x[i] = 0;
			al_draw_bitmap(buses[i], buses_x[i], buses_y[i], 0);
			
			if( (bouncer_x + BOUNCER_SIZE > buses_x[i] && bouncer_x < buses_x[i] + buses_comp[i]) && (bouncer_y < buses_y[i] + LARGURA_BUS && bouncer_y > buses_y[i]) ) {
				playing = 0;
				collision = 1;
			}
			
		}		 
		//reinicializo a tela
		 al_flip_display();
    }
	//se o tipo de evento for o fechamento da tela (clique no x da janela)
	else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
		//interrompe o while(1)
		break;
	}
 
  } //fim do while
  
	//inicializa o modulo allegro que carrega as fontes
    al_init_font_addon();
	//inicializa o modulo allegro que entende arquivos tff de fontes
    al_init_ttf_addon();
	//carrega o arquivo arial.ttf da fonte Arial e define que sera usado o tamanho 32 (segundo parametro)
    ALLEGRO_FONT *size_32 = al_load_font("arial.ttf", 32, 1);	
	
	char my_text[20];	
		
	//colore toda a tela de preto
	al_clear_to_color(al_map_rgb(0,0,0));
	//imprime o texto armazenado em my_text na posicao x=10,y=10 e com a cor rgb(128,200,30)
	if(collision)
		al_draw_text(size_32, al_map_rgb(200, 0, 30), SCREEN_W/3, SCREEN_H/2, 0, "Perdeu :(");
	else {
		sprintf(my_text, "Ganhou: %.2f segundos", al_get_timer_count(timer)/FPS);	
		al_draw_text(size_32, al_map_rgb(0, 200, 30), SCREEN_W/3, SCREEN_H/2, 0, my_text);
	}
	
	//reinicializa a tela
	al_flip_display();	
    al_rest(3);		
   
	//procedimentos de fim de jogo (fecha a tela, limpa a memoria, etc)
	
	for(i=0; i<NUM_RUAS; i++) 
		al_destroy_bitmap(buses[i]);
   al_destroy_bitmap(bouncer);
   al_destroy_timer(timer);
   al_destroy_display(display);
   al_destroy_event_queue(event_queue);
 
   return 0;
}