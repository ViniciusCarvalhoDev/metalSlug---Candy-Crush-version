#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <stdlib.h>
#include <math.h>

#define MAXTILES 5
#define AVG_TILES_PER_LANE 20
#define MAXLANES 6
#define MIN_SCORE -10*NUM_LANES

#define CREATED 0
#define HIT -1
#define ACTIVE 1
#define LIMBO -2



const int FPS = 180;  
const int SCREEN_W = 960;
const int SCREEN_H = 640;

int SKY_H;
int TILE_HIT_H;
int TILE_R = 50;

int LANE_W;
int NUM_LANES=3;

int TRACK_W;
int TRACK_LEFT_X;
int TRACK_RIGHT_X;
int KEY1 = ALLEGRO_KEY_1;

int score = 0;

typedef struct Tile {
	float x,y; //x center, y bottom 
	int lane; 
	int status;
	int delay;
	int id;
	ALLEGRO_COLOR color;
} Tile;



void init_track() {
	SKY_H = SCREEN_H * 0.25;
	LANE_W = (2*TILE_R) * 1.5;
	TRACK_W = NUM_LANES * LANE_W;
	TRACK_LEFT_X = SCREEN_W/2 - TRACK_W/2;
	TRACK_RIGHT_X = TRACK_LEFT_X + TRACK_W;
}

void draw_scenario(ALLEGRO_DISPLAY *display) {
	
	int i;
	
	//screen
	ALLEGRO_COLOR BKG_COLOR = al_map_rgb(0,0,0); 
	al_set_target_bitmap(al_get_backbuffer(display));
	al_clear_to_color(BKG_COLOR);   
	
	//sky
	al_draw_filled_rectangle(0, 0, SCREEN_W, SKY_H, al_map_rgb(100,100,100));

  
   //desenha a pista:
   al_draw_line(TRACK_LEFT_X, SKY_H, TRACK_LEFT_X, SCREEN_H, al_map_rgb(255,255,255), 10); 
   al_draw_line(TRACK_RIGHT_X, SKY_H, TRACK_RIGHT_X, SCREEN_H, al_map_rgb(255,255,255), 10); 
   for(i=1; i<NUM_LANES; i++) {
	   al_draw_line(TRACK_LEFT_X+(LANE_W*i), SKY_H, TRACK_LEFT_X+(LANE_W*i), SCREEN_H, al_map_rgb(255,255,255), 3); 
   }
   
}



int main(int argc, char **argv){
	

	ALLEGRO_DISPLAY *display = NULL;
	ALLEGRO_EVENT_QUEUE *event_queue = NULL;
	ALLEGRO_TIMER *timer = NULL;
	int i=0;

	char my_score[10];

 
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

	event_queue = al_create_event_queue();
   if(!event_queue) {
      fprintf(stderr, "failed to create event_queue!\n");
      al_destroy_display(display);
      al_destroy_timer(timer);
      return -1;
   }
   
   al_install_keyboard();
   
	//registra na fila de eventos que eu quero identificar quando a tela foi alterada
   al_register_event_source(event_queue, al_get_display_event_source(display));
	//registra na fila de eventos que eu quero identificar quando o tempo alterou de t para t+1
   al_register_event_source(event_queue, al_get_timer_event_source(timer));
   
   al_register_event_source(event_queue, al_get_keyboard_event_source());   
   
  

	//reinicializa a tela
   al_flip_display();
	//inicia o temporizador
   al_start_timer(timer);

	init_track();

	
   int playing = 1;
	//enquanto playing for verdadeiro, faca:
   while(playing) {
      ALLEGRO_EVENT ev;
	  //espera por um evento e o armazena na variavel de evento ev
      al_wait_for_event(event_queue, &ev);
	  
	if(ev.type == ALLEGRO_EVENT_KEY_DOWN) {
		printf("\n%d", ev.keyboard.keycode);
		
		//para cada tecla que eu apertar, eu ganho 1 ponto
		score++;


		
		if(ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
			playing = 0;
		}
 
   }
	//se o tipo de evento for um evento do temporizador, ou seja, se o tempo passou de t para t+1
    else if(ev.type == ALLEGRO_EVENT_TIMER) {
		draw_scenario(display);
		
		
		//SCORE
		sprintf(my_score, "%d", score);
        al_draw_text(size_32, al_map_rgb(255, 0, 0), SCREEN_W-100, 20, 0, my_score);
		

		//reinicializo a tela
		 al_flip_display();

			 

    }
	//se o tipo de evento for o fechamento da tela (clique no x da janela)
	else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
		playing = 0;
	}
	
  } //fim do while
     
	//procedimentos de fim de jogo (fecha a tela, limpa a memoria, etc)
	
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