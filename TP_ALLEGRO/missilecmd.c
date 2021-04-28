#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <stdlib.h>
#include <math.h>

#define MAX_BOMBS 5
#define MAX_MISSILES 10
#define SPEED_MISSILE 3

const float FPS = 100;  
const int SCREEN_W = 640*1.5;
const int SCREEN_H = 540;

const float BASE_W = 100;
const float BASE_H = 50;

const float GRASS_H = 20;

int score = 0;

typedef struct Bomb {
	int x,y;
	float radius;
	int active;
	float xmissile, ymissile;
} Bomb;

typedef struct Missile {
	int xSrc, ySrc;
	float x, y;
	int xDst, yDst;
	float speed;
	float delay;
} Missile;


void draw_scenario(ALLEGRO_DISPLAY *display) {
	ALLEGRO_COLOR BKG_COLOR = al_map_rgb(255,255,255); 
    
	//avisa o allegro que agora eu quero modificar as propriedades da tela
   al_set_target_bitmap(al_get_backbuffer(display));
	//colore a tela de preto (rgb(0,0,0))
   al_clear_to_color(BKG_COLOR);   
   
   al_draw_filled_rectangle(0, SCREEN_H, SCREEN_W, SCREEN_H - GRASS_H, al_map_rgb(0,255,0));
   al_draw_filled_triangle(SCREEN_W/2.0 - BASE_W/2.0, SCREEN_H, SCREEN_W/2.0 + BASE_W/2.0, SCREEN_H, SCREEN_W/2.0, SCREEN_H - BASE_H, al_map_rgb(255,2,55));
   
}



 
int main(int argc, char **argv){
	
	srand(time(NULL));
	ALLEGRO_DISPLAY *display = NULL;
	ALLEGRO_EVENT_QUEUE *event_queue = NULL;
	ALLEGRO_TIMER *timer = NULL;
   
	//----------------------- rotinas de inicializacao ---------------------------------------
   if(!al_init()) {
      fprintf(stderr, "failed to initialize allegro!\n");
      return -1;
   }
   
    if(!al_init_primitives_addon()){
		fprintf(stderr, "failed to initialize primitives!\n");
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


	//inicializa o modulo allegro que carrega as fontes
    al_init_font_addon();
	//inicializa o modulo allegro que entende arquivos tff de fontes
    al_init_ttf_addon();


	//carrega o arquivo arial.ttf da fonte Arial e define que sera usado o tamanho 32 (segundo parametro)
    ALLEGRO_FONT *size_32 = al_load_font("arial.ttf", 32, 1);   	

 
	ALLEGRO_COLOR BKG_COLOR = al_map_rgb(255,255,255);
   
    
	//avisa o allegro que agora eu quero modificar as propriedades da tela
   al_set_target_bitmap(al_get_backbuffer(display));
	//colore a tela de preto (rgb(0,0,0))
   al_clear_to_color(BKG_COLOR);   
 
	//cria a fila de eventos
   event_queue = al_create_event_queue();
   if(!event_queue) {
      fprintf(stderr, "failed to create event_queue!\n");
      al_destroy_display(display);
      al_destroy_timer(timer);
      return -1;
   }
 
	//registra na fila de eventos que eu quero identificar quando a tela foi alterada
   al_register_event_source(event_queue, al_get_display_event_source(display));
	//registra na fila de eventos que eu quero identificar quando o tempo alterou de t para t+1
   al_register_event_source(event_queue, al_get_timer_event_source(timer));
   
  

	//reinicializa a tela
   al_flip_display();
	//inicia o temporizador
   al_start_timer(timer);

   int playing = 1;
	//enquanto playing for verdadeiro, faca:
   while(playing) {
      ALLEGRO_EVENT ev;
	  //espera por um evento e o armazena na variavel de evento ev
      al_wait_for_event(event_queue, &ev);
	 
	
	//se o tipo de evento for um evento do temporizador, ou seja, se o tempo passou de t para t+1
    if(ev.type == ALLEGRO_EVENT_TIMER) {
		
		draw_scenario(display);
		

		//reinicializo a tela
		 al_flip_display();
		 
		if(al_get_timer_count(timer)%(int)FPS == 0)
			printf("\n%d segundos se passaram...", (int)(al_get_timer_count(timer)/FPS));
		 
		 

    }
	//se o tipo de evento for o fechamento da tela (clique no x da janela)
	else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
		playing = 0;
	}
	
  } //fim do while
     
	//procedimentos de fim de jogo (fecha a tela, limpa a memoria, etc)
	
	al_rest(1);
	
	char my_text[20];	
	int record;
	//colore toda a tela de preto
	al_clear_to_color(al_map_rgb(230,240,250));
	sprintf(my_text, "Score: %d", score);
	al_draw_text(size_32, al_map_rgb(200, 0, 30), SCREEN_W/3, SCREEN_H/2, 0, my_text);

	
	//reinicializa a tela
	al_flip_display();	
    al_rest(2);	
	
  
   al_destroy_timer(timer);
   al_destroy_display(display);
   al_destroy_event_queue(event_queue);
   
 
   return 0;
}