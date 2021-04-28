#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

//este exemplo foi tirado da pagina http://wiki.allegro.cc/index.php?title=Allegro_5_Tutorial/Bitmaps
//veja a pagina para mais detalhes
 
const float FPS = 60;
const int SCREEN_W = 640;
const int SCREEN_H = 480;
//variavel global que armazena o tamanho do bouncer (quadrado que vai navegar pela tela)
const int BOUNCER_SIZE = 16;

//players
const int PLAYER_H = 80;
const int PLAYER_W = 20;

float flip_bouncer(float bouncer_dx) {
	int s=1;
	if(bouncer_dx < 0) s=-1;
	bouncer_dx = bouncer_dx*(0.5+2*(float)rand()/RAND_MAX);
	if(s*bouncer_dx < 2)
		bouncer_dx = 2*s;
	else if(s*bouncer_dx > 20)
		bouncer_dx = 20*s;
	return -bouncer_dx;
}


 
int main(int argc, char **argv){
   ALLEGRO_DISPLAY *display = NULL;
   ALLEGRO_EVENT_QUEUE *event_queue = NULL;
   ALLEGRO_TIMER *timer = NULL;
   ALLEGRO_BITMAP *bouncer = NULL;
   
   float p2_x = SCREEN_W - PLAYER_W - 5;
   float p1_x = 5;
   int count = 0;
   int winner = 2;
   
   //posicoes x e y iniciais do bouncer
   float bouncer_x = SCREEN_W / 2.0 - BOUNCER_SIZE / 2.0;
   float bouncer_y = SCREEN_H / 2.0 - BOUNCER_SIZE / 2.0;
   
   //o quanto as posicoes x e y vao variar ao longo do tempo. No t=1, se x do bouncer eh 40, no t=2, x do bouncer eh 40 + bouncer_dx = 36
   float bouncer_dx = -2.0, bouncer_dy = 2.0;
   bool redraw = true;
   bool jogando = true;
   
   
	//players
	ALLEGRO_BITMAP *p1=NULL, *p2=NULL;
	float p1_y = 0, p2_y=0;
	float p1_dy = 4, p2_dy = 4;
	bool p1_move_down = false;
	bool p1_move_up = false;
 
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
 
	//cria um bitmap quadrangular de tamanho BOUNCER_SIZE (variavel global declarada acima) e os players
   bouncer = al_create_bitmap(BOUNCER_SIZE, BOUNCER_SIZE);
   p1 = al_create_bitmap(PLAYER_W, PLAYER_H);
   p2 = al_create_bitmap(PLAYER_W, PLAYER_H);   
   if(!bouncer || !p1 || !p2) {
      fprintf(stderr, "failed to create bouncer bitmap!\n");
      al_destroy_display(display);
      al_destroy_timer(timer);
      return -1;
   }
   
    al_install_keyboard();
    al_init_primitives_addon();
 
	//avisa o allegro que eu quero modificar as propriedades do bouncer
   al_set_target_bitmap(bouncer);
	//altera a cor do bouncer para rgb(255,0,255)
   al_clear_to_color(al_map_rgb(255, 0, 255));
   
	//avisa o allegro que eu quero modificar as propriedades do p1
   al_set_target_bitmap(p1);
   al_clear_to_color(al_map_rgb(255, 0, 0));   
   
	//avisa o allegro que eu quero modificar as propriedades do p2
   al_set_target_bitmap(p2);
   al_clear_to_color(al_map_rgb(0, 0, 255));      
 
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
   al_register_event_source(event_queue, al_get_keyboard_event_source());   
 

	//reinicializa a tela
   al_flip_display();
	//inicia o temporizador
   al_start_timer(timer);
 
	//loop infinito, ou seja, enquanto 1 for verdadeiro, faca:
   while(jogando)
   {
      ALLEGRO_EVENT ev;
	  //espera por um evento e o armazena na variavel de evento ev
      al_wait_for_event(event_queue, &ev);
 
		//se o tipo de evento for um evento do temporizador, ou seja, se o tempo passou de t para t+1
      if(ev.type == ALLEGRO_EVENT_TIMER) {
	  
		if(bouncer_x <= p1_x+PLAYER_W && bouncer_x >= p1_x && bouncer_y <= p1_y+PLAYER_H && bouncer_y + BOUNCER_SIZE >= p1_y) {
			bouncer_x = p1_x+PLAYER_W;
			bouncer_dx = flip_bouncer(bouncer_dx);
			printf("\nbouncer_dx = %f", bouncer_dx);
			count++;
		 }

		else if(bouncer_x + BOUNCER_SIZE <= p2_x + PLAYER_W && bouncer_x +BOUNCER_SIZE >= p2_x && bouncer_y <= p2_y+PLAYER_H && bouncer_y + BOUNCER_SIZE >= p2_y) {
			bouncer_x = p2_x-BOUNCER_SIZE;
			bouncer_dx = flip_bouncer(bouncer_dx);
			count++;
		 }
	  
			//verifica se a posicao x do bouncer passou dos limites da tela
        else if(bouncer_x < 0) {
			winner = 2;
			jogando = false;
		}
		
		else if(bouncer_x > SCREEN_W - BOUNCER_SIZE) {
			//altera a direcao na qual o bouncer se move no eixo x
            //bouncer_dx = -bouncer_dx;
			winner = 1;
			jogando = false;
         }
		 //verifica se a posicao y do bouncer passou dos limites da tela
        else if(bouncer_y < 0 || bouncer_y > SCREEN_H - BOUNCER_SIZE) {
			//altera a direcao na qual o bouncer se move no eixo y
            bouncer_dy = -bouncer_dy;
         }
		 

		 
		 
		 
		 
 
		//faz o bouncer se mover no eixo x e y incrementando as suas posicoes de bouncer_dx e bouncer_dy, respectivamente
         bouncer_x += bouncer_dx;
         bouncer_y += bouncer_dy;
 
         redraw = true;
      }
	  //se o tipo de evento for o fechamento da tela (clique no x da janela)
      else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
		//interrompe o while(1)
         break;
      }
      else if(ev.type == ALLEGRO_EVENT_KEY_DOWN) {
	
		//verifica qual tecla foi
		switch(ev.keyboard.keycode) {
			//se a tecla for o W
			case ALLEGRO_KEY_W:
				p1_move_up = true;
			break;

			//se a tecla for o S
			case ALLEGRO_KEY_S:
				p1_move_down = true;
			break;
		}
	}	
	else if(ev.type == ALLEGRO_EVENT_KEY_UP) {
	
		//verifica qual tecla foi
		switch(ev.keyboard.keycode) {
			//se a tecla for o W
			case ALLEGRO_KEY_W:
				p1_move_up = false;
			break;

			//se a tecla for o S
			case ALLEGRO_KEY_S:
				p1_move_down = false;
			break;
		}
	}


		//se eu alterei a posicao do bouncer, o redraw foi para true e eu nao tenho eventos na fila para ler
      if(redraw && al_is_event_queue_empty(event_queue)) {

         redraw = false;
		//limpo a tela
         al_clear_to_color(al_map_rgb(0,0,0));
		//desenho o bouncer nas novas posicoes x e y
         al_draw_bitmap(bouncer, bouncer_x, bouncer_y, 0);
		 
		 //desenha players
		if(p1_y > 0 && p1_move_up)
			p1_y -= p1_dy;
		else if(p1_y + PLAYER_H < SCREEN_H && p1_move_down)
			p1_y += p1_dy;
			
		 al_draw_bitmap(p1, p1_x, p1_y, 0);
		 
		if(bouncer_y > p2_y + PLAYER_H/2 && p2_y + PLAYER_H < SCREEN_H) {
			p2_y += p2_dy;
		}
		else if(p2_y > 0)
			p2_y -= p2_dy;
		 
		al_draw_bitmap(p2, p2_x, p2_y, 0);
		 
		//reinicializo a tela
         al_flip_display();
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
	sprintf(my_text, "P%d ganhou!", winner);	
	al_draw_text(size_32, al_map_rgb(15, 200, 30), SCREEN_W/3, SCREEN_H/2, 0, my_text);
	sprintf(my_text, "Total de rebatidas: %d", count);	
	al_draw_text(size_32, al_map_rgb(15, 200, 30), SCREEN_W/3, SCREEN_H/2+100, 0, my_text);	
	
	
	//reinicializa a tela
	al_flip_display();	
    al_rest(3);		   
   
	//procedimentos de fim de jogo (fecha a tela, limpa a memoria, etc)
   al_destroy_bitmap(bouncer);
   al_destroy_bitmap(p1);
   al_destroy_bitmap(p2);
   al_destroy_timer(timer);
   al_destroy_display(display);
   al_destroy_event_queue(event_queue);
 
   return 0;
}