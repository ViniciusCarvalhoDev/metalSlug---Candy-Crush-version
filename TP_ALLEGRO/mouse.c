#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include<allegro5/allegro_primitives.h>
#include <stdlib.h>




const float FPS = 100;  
const int SCREEN_W = 640*1.5;
const int SCREEN_H = 540;



void handle_click(int x, int y) {
	al_draw_filled_circle(x, y, 1+rand()%10, al_map_rgb(255,2,25));
	
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

	if(!al_install_mouse())
		fprintf(stderr, "failed to initialize mouse!\n");

 
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
	// Registra mouse na fila de eventos:
	al_register_event_source(event_queue, al_get_mouse_event_source());   
   
  

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
	 
	if(ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
			handle_click(ev.mouse.x, ev.mouse.y);
	}
	
	//se o tipo de evento for um evento do temporizador, ou seja, se o tempo passou de t para t+1
    else if(ev.type == ALLEGRO_EVENT_TIMER) {
		

	 
		//reinicializo a tela
		 al_flip_display();
		 
		 

    }
	//se o tipo de evento for o fechamento da tela (clique no x da janela)
	else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
		playing = 0;
	}
	
  } //fim do while
     

	
  
   al_destroy_timer(timer);
   al_destroy_display(display);
   al_destroy_event_queue(event_queue);
 
   return 0;
}