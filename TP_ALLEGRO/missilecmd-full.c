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
const int MAX_RADIUS = 50;

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

int newRecord(int score, int *record) {
	FILE *arq = fopen("recorde.txt", "r");
	*record = -1;
	if(arq != NULL) {
		fscanf(arq, "%d", record);
		fclose(arq);
	}
	if(*record < score ) {
		arq = fopen("recorde.txt", "w");
		fprintf(arq, "%d", score);
		fclose(arq);
		return 1;
	}
	return 0;
	
}

void createMissile(Missile *m) {
	m->xSrc = rand()%SCREEN_W;
	m->ySrc = 0;
	m->x = m->xSrc;
	m->y = m->ySrc;
	m->xDst = rand()%SCREEN_W;
	m->yDst = SCREEN_H;
	m->speed = (float)rand()/RAND_MAX; // * (rand()%3);
	m->delay = 4+rand()%MAX_MISSILES;
}

int xLauncher() {
	return SCREEN_W/2.0;
}

int yLauncher() {
	return SCREEN_H - BASE_H;
}

float dist(float x1, float y1, float x2, float y2) {
	return sqrt(pow(x1-x2, 2) + pow(y1-y2, 2));
}

void handle_click(int x, int y, Bomb bombs[], int *nbombs) {
	if(*nbombs == MAX_BOMBS)
		return;
	bombs[*nbombs].active = 0;
	bombs[*nbombs].radius = 0;
	bombs[*nbombs].x = x;
	bombs[*nbombs].y = y;
	bombs[*nbombs].xmissile = xLauncher();
	bombs[*nbombs].ymissile = yLauncher();	
	(*nbombs)++;
	
	
}

void arrange_bombs_vector(Bomb bombs[], int idx, int *nbombs) {
	int i=0;
	for(i=idx; i<*nbombs-1; i++) {
		bombs[i] = bombs[i+1];
	}
	(*nbombs)--;
}

void arrange_bombs(Bomb bombs[], int *nbombs) {
	int i=0;
	while(i < *nbombs) {
		if(bombs[i].radius <= MAX_RADIUS) {
			i++;
		}
		else {
			arrange_bombs_vector(bombs, i, nbombs);
		}
	}
}

int isCollision(Bomb bomb, Missile missile) {
	float d = dist(missile.x, missile.y, bomb.x, bomb.y);
	if(d < bomb.radius)
		return 1;
	return 0;
}

void checkCollisions(Bomb bombs[], int nbombs, Missile missiles[]) {
	int i,j;
	for(i=0; i<nbombs; i++) {
		for(j=0; j<MAX_MISSILES; j++) {
			if(missiles[j].delay<=0 && isCollision(bombs[i], missiles[j])) {
				score++;
				bombs[i].active = 0;
				createMissile(&missiles[j]);
			}
		}
	}
}

int checkMissiles(Missile missiles[]) {
	int j;
	for(j=0; j<MAX_MISSILES; j++) {
		if(missiles[j].y > SCREEN_H - GRASS_H) {
			return 0;
		}
	}
	return 1;
}

void draw_missiles(Missile missiles[]) {
	int i;
	for(i=0; i<MAX_MISSILES; i++) {
		if(missiles[i].delay <= 0) {
			float h = dist(missiles[i].xSrc, missiles[i].ySrc, missiles[i].xDst, missiles[i].yDst);
			float catx = (missiles[i].xSrc - missiles[i].xDst);
			float caty = (missiles[i].ySrc - missiles[i].yDst);
			missiles[i].x -= missiles[i].speed*catx/h;
			missiles[i].y -= missiles[i].speed*caty/h;
			al_draw_line(missiles[i].xSrc, missiles[i].ySrc, missiles[i].x, missiles[i].y, al_map_rgb(230,150,30), 2);
		}
		else {
			missiles[i].delay -= 1.0/FPS;
		}

	}
}

void draw_bombs(Bomb bombs[], int nbombs) {
	int i=0;
	for(i=0; i< nbombs; i++) {
		if(bombs[i].radius > MAX_RADIUS) {
			bombs[i].active = 0;
		}
		else if(bombs[i].active) {
			al_draw_filled_circle(bombs[i].x, bombs[i].y, bombs[i].radius++, al_map_rgb(rand()%256,rand()%256,rand()%256));
		}
		else if(dist(bombs[i].x,bombs[i].y,bombs[i].xmissile,bombs[i].ymissile)<=5) {
			bombs[i].active = 1;
		}
		else{
			float h = dist(bombs[i].x, bombs[i].y, xLauncher(), yLauncher());
			float catx = (bombs[i].x - xLauncher());
			float caty = (bombs[i].y - yLauncher());
			bombs[i].xmissile += SPEED_MISSILE*catx/h;
			bombs[i].ymissile += SPEED_MISSILE*caty/h;
			al_draw_line(xLauncher(), yLauncher(), bombs[i].xmissile, bombs[i].ymissile, al_map_rgb(255,0,0), 2);
		}
	}
}

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
	char my_score[10];
	Bomb bombs[MAX_BOMBS];
	Missile missiles[MAX_MISSILES];
	
	int i,nbombs=0;
	
	for(i=0; i<MAX_MISSILES; i++) {
		createMissile(&missiles[i]);
	}


	
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
			handle_click(ev.mouse.x, ev.mouse.y, bombs, &nbombs);
	}
	
	//se o tipo de evento for um evento do temporizador, ou seja, se o tempo passou de t para t+1
    else if(ev.type == ALLEGRO_EVENT_TIMER) {
		
		draw_scenario(display);
		draw_missiles(missiles);
		draw_bombs(bombs, nbombs);
		checkCollisions(bombs, nbombs, missiles);
		playing = checkMissiles(missiles);
		arrange_bombs(bombs, &nbombs);
		
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
	
	al_rest(1);
	
	char my_text[20];	
	int record;
	//colore toda a tela de preto
	al_clear_to_color(al_map_rgb(230,240,250));
	sprintf(my_text, "Score: %d", score);
	al_draw_text(size_32, al_map_rgb(200, 0, 30), SCREEN_W/3, SCREEN_H/2, 0, my_text);
	if(newRecord(score, &record)) {
		al_draw_text(size_32, al_map_rgb(200, 20, 30), SCREEN_W/3, 100+SCREEN_H/2, 0, "NEW RECORD!");
	}
	else {
		sprintf(my_text, "Record: %d", record);
		al_draw_text(size_32, al_map_rgb(0, 200, 30), SCREEN_W/3, 100+SCREEN_H/2, 0, my_text);
	}



	
	//reinicializa a tela
	al_flip_display();	
    al_rest(2);	
	
  
   al_destroy_timer(timer);
   al_destroy_display(display);
   al_destroy_event_queue(event_queue);
   
 
   return 0;
}