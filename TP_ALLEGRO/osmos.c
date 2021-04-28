#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include<allegro5/allegro_primitives.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define SPEED_CORRECTION 0.5
#define MAX_PASSIVES 100
#define PASSIVE_START_COUNT 30
#define INITIAL_RADIUS 40
#define RELATIVE_PASSIVE_AREA 0.03
#define ENEMY_RADIUS_RATIO 1
#define ENEMY_MAX_SPEED 1
#define RANDOM_PASSIVE_MAX_RADIUS 10
#define RANDOM_PASSIVE_MAX_SPEED 0.5
#define PASSIVE_CREATION_PROB 0.01


const float FPS = 100;  
const int SCREEN_W = 640*1.5;
const int SCREEN_H = 540;



typedef struct Ship {
	float x;
	float y;
	float radius;
	ALLEGRO_COLOR color;
	float xspeed;
	float yspeed;
	int alive;
} Ship;

int newRecord(float score, float *record) {
	FILE *arq = fopen("recorde.txt", "r");
	*record = -1;
	if(arq != NULL) {
		fscanf(arq, "%f", record);
		fclose(arq);
	}
	if(*record > score ) {
		arq = fopen("recorde.txt", "w");
		fprintf(arq, "%.2f", score);
		fclose(arq);
		return 1;
	}
	return 0;
	
}


float getRadiusPassive(float *radiusShip) {
	float area = M_PI * pow(*radiusShip,2);
	float areaPassive = area * RELATIVE_PASSIVE_AREA;
	*radiusShip = sqrt((area - areaPassive)/M_PI);
	return sqrt(areaPassive/M_PI);
}

float dist(float x1, float y1, float x2, float y2) {
	return sqrt(pow(x1 - x2,2) + pow(y1 - y2,2));
}

float randFloat(float max) {
	return (float)rand()/RAND_MAX * max;
}

float getRadiusFromMerge(float r1, float r2) {
	return sqrt( pow(r1,2) + pow(r2,2) );
}

void createPassive(Ship *passive, float radius, float x, float y, float xspeed, float yspeed) {
	passive->x = x;
	passive->y = y;
	passive->radius = radius;
	passive->xspeed = xspeed;
	passive->yspeed = yspeed;
	passive->color = al_map_rgb(rand()%50, rand()%100, rand()%100);
}

void destroyPassive(Ship passives[], int passiveIdx, int *npassives) {
	int i;
	for(i=passiveIdx; i<*npassives-1; i++)
		passives[i] = passives[i+1];
	*npassives = *npassives - 1;
}



int collision(Ship ship, Ship passive) {
	//printf("\ntesting collision...");
	if(dist(ship.x, ship.y, passive.x, passive.y) < passive.radius + ship.radius)
		return 1;
	return 0;
}

void handlePassiveCollision(Ship *ship1, Ship *ship2) {
	if(ship1->radius < ship2->radius ) {
		Ship aux = *ship2;
		*ship2 = *ship1;
		*ship1 = aux;
	}
}

void handleCollisionShipPassive(Ship *ship, Ship passives[], int *npassives, int passiveIdx) {
	if(ship->radius > passives[passiveIdx].radius) {
		ship->radius = getRadiusFromMerge(ship->radius, passives[passiveIdx].radius);
		destroyPassive(passives, passiveIdx, npassives);
	}
	else {
		passives[passiveIdx].radius = getRadiusFromMerge(ship->radius, passives[passiveIdx].radius);
		ship->alive = 0;
	}
}


void verifyCollisions(Ship *ship, Ship *enemy, Ship passives[], int *npassives) {
	int i,j;
	if(collision(*ship, *enemy)) {
		if(ship->radius > enemy->radius) {
			enemy->alive = 0;
			ship->radius = getRadiusFromMerge(ship->radius, enemy->radius);
			enemy->radius = 0.1;
		}
		else {
			ship->alive = 0;
			enemy->radius = getRadiusFromMerge(ship->radius, enemy->radius);	
			ship->radius = 0.1;
		}
	}
		
	
	for(i=0; i<*npassives; i++) {
		if(collision(*ship, passives[i])) {
			handleCollisionShipPassive(ship, passives, npassives, i);
		}
		else if(collision(*enemy, passives[i])) {
			handleCollisionShipPassive(enemy, passives, npassives, i);
		}		
		else {
			for(j=i+1; j<*npassives; j++) {
				if(collision(passives[i], passives[j])) {
					handlePassiveCollision(&passives[i], &passives[j]);
					passives[i].radius = getRadiusFromMerge(passives[i].radius, passives[j].radius);
					//passives[i].radius += passives[j].radius;
					destroyPassive(passives, j, npassives);
				}
			}
		}
	}
}


void handle_click(Ship *ship, int x, int y, Ship passives[], int *npassives) {
	if(*npassives < MAX_PASSIVES && ship->radius > 1) {
		float dx = ship->x - x;
		float dy = ship->y - y;
		float h = dist(ship->x, ship->y, x, y);
		float xcomp = dx/h * SPEED_CORRECTION;
		float ycomp = dy/h * SPEED_CORRECTION;
		float passivex = ship->x - 3*(ship->radius * xcomp);
		float passivey = ship->y - 3*(ship->radius * ycomp);
		createPassive(&passives[*npassives], getRadiusPassive(&ship->radius), passivex, passivey, ship->xspeed-xcomp, ship->yspeed-ycomp);
		
		/*
		createPassive(&passives[*npassives], ship->radius/5, passivex, passivey, ship->xspeed-xcomp, ship->yspeed-ycomp);
		ship->radius -= passives[*npassives].radius;
		*/
		ship->xspeed += xcomp;
		ship->yspeed += ycomp;
		*npassives = *npassives + 1;
	}
	
}


void drawShip(Ship ship) {
	
	al_draw_filled_circle(ship.x, ship.y, ship.radius, ship.color);
}

void drawPassives(Ship passives[], int npassives) {
	int i;
	for(i=0; i<npassives; i++) {
		al_draw_filled_circle(passives[i].x, passives[i].y, passives[i].radius, passives[i].color);
	}	
}

void moveShip(Ship *ship) {
		ship->x += ship->xspeed;
		ship->y += ship->yspeed;
		if(ship->x+ship->radius > SCREEN_W || ship->x-ship->radius < 0) {
			ship->xspeed = -ship->xspeed;
		}
		if(ship->y+ship->radius > SCREEN_H || ship->y-ship->radius < 0) {
			ship->yspeed = -ship->yspeed;
		}
}

void movePassives(Ship passives[], int npassives) {
	int i;
	for(i=0; i<npassives; i++) {
		moveShip(&passives[i]);
	}
	
}




 
int main(int argc, char **argv){
	
	srand(time(NULL));
	
	ALLEGRO_DISPLAY *display = NULL;
	ALLEGRO_EVENT_QUEUE *event_queue = NULL;
	ALLEGRO_TIMER *timer = NULL;
	Ship ship;
	Ship passives[MAX_PASSIVES];
	Ship enemy;
	int npassives = 0;
	float score=0, record;
	char my_score[10];

	

	int i,j,playing=1;

  
   
   
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
	
	if(!al_install_mouse())
		fprintf(stderr, "failed to initialize mouse!\n");

 
	ALLEGRO_COLOR BKG_COLOR = al_map_rgb(255,255,255);
   
   al_install_keyboard();
   
	//create ship
	ship.x = (SCREEN_W)/2.0;
	ship.y = (SCREEN_H)/2.0;
	ship.radius = INITIAL_RADIUS;
	ship.xspeed = 0;
	ship.yspeed = 0;
	ship.color = al_map_rgb(0, 255, 0);
	ship.alive = 1;
	
	//create enemy
	enemy.x = (SCREEN_W)-2*INITIAL_RADIUS;
	enemy.y = (SCREEN_H)-2*INITIAL_RADIUS;
	enemy.radius = INITIAL_RADIUS*ENEMY_RADIUS_RATIO;
	enemy.xspeed = (pow(-1,rand()%2))*randFloat(ENEMY_MAX_SPEED);
	enemy.yspeed = (pow(-1,rand()%2))*randFloat(ENEMY_MAX_SPEED);
	enemy.color = al_map_rgb(255, 0, 0);
	enemy.alive = 1;	
	
	//create random passives
	for(npassives=0; npassives<PASSIVE_START_COUNT; npassives++) {
		createPassive(&passives[npassives], 1+rand()%RANDOM_PASSIVE_MAX_RADIUS, rand()%SCREEN_W, rand()%SCREEN_H, (pow(-1,rand()%2))*randFloat(RANDOM_PASSIVE_MAX_SPEED), (pow(-1,rand()%2))*randFloat(RANDOM_PASSIVE_MAX_SPEED));
	}

	
	   
    
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
   //registra que a fila de eventos deve detectar quando uma tecla for pressionada no teclado
   al_register_event_source(event_queue, al_get_keyboard_event_source());  
	// Registra mouse na fila de eventos:
	al_register_event_source(event_queue, al_get_mouse_event_source());   
   
  

	//reinicializa a tela
   al_flip_display();
	//inicia o temporizador
   al_start_timer(timer);

	//enquanto playing for verdadeiro, faca:
   while(ship.alive && enemy.alive) {
      ALLEGRO_EVENT ev;
	  //espera por um evento e o armazena na variavel de evento ev
      al_wait_for_event(event_queue, &ev);
	 
	if(ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
			handle_click(&ship, ev.mouse.x, ev.mouse.y, passives, &npassives);
	}
	
	//se o tipo de evento for um evento do temporizador, ou seja, se o tempo passou de t para t+1
    else if(ev.type == ALLEGRO_EVENT_TIMER) {
		
		//limpo a tela
		al_clear_to_color(BKG_COLOR);
		//desenho o ship nas novas posicoes x e y

		moveShip(&ship);
		moveShip(&enemy);
		movePassives(passives, npassives);
		verifyCollisions(&ship, &enemy, passives, &npassives);
		
		//create random passive?
		if(npassives < MAX_PASSIVES) {
			if(randFloat(1) < PASSIVE_CREATION_PROB) {
				createPassive(&passives[npassives], RANDOM_PASSIVE_MAX_RADIUS, rand()%SCREEN_W, rand()%SCREEN_H, (pow(-1,rand()%2))*randFloat(RANDOM_PASSIVE_MAX_SPEED), (pow(-1,rand()%2))*randFloat(RANDOM_PASSIVE_MAX_SPEED));
				npassives++;
			}
		}
		
		drawShip(ship);
		drawShip(enemy);
		drawPassives(passives, npassives);
		
		//SCORE
		score = al_get_timer_count(timer)/FPS;
		sprintf(my_score, "%.2f", score);
        //imprime o texto armazenado em my_text na posicao x=10,y=10 e com a cor rgb(128,200,30)
        al_draw_text(size_32, al_map_rgb(255, 0, 0), SCREEN_W-100, 20, 0, my_score);
		 
		//reinicializo a tela
		 al_flip_display();
		 
		 

    }
	//se o tipo de evento for o fechamento da tela (clique no x da janela)
	else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
		ship.alive = 0;
	}
	
  } //fim do while
     
	//procedimentos de fim de jogo (fecha a tela, limpa a memoria, etc)
	
	al_rest(1);
	
	char my_text[20];	
	//colore toda a tela de preto
	al_clear_to_color(al_map_rgb(230,240,250));
	if(ship.alive) {
		//imprime o texto armazenado em my_text na posicao x=10,y=10 e com a cor rgb(128,200,30)
		sprintf(my_text, "Score: %.2f", score);
		al_draw_text(size_32, al_map_rgb(200, 0, 30), SCREEN_W/3, SCREEN_H/2, 0, my_text);
		if(newRecord(score, &record)) {
			al_draw_text(size_32, al_map_rgb(200, 20, 30), SCREEN_W/3, 100+SCREEN_H/2, 0, "NEW RECORD!");
		}
		else {
			sprintf(my_text, "Record: %.2f", record);
			al_draw_text(size_32, al_map_rgb(0, 200, 30), SCREEN_W/3, 100+SCREEN_H/2, 0, my_text);
		}
	}
	else {
		sprintf(my_text, "You lost. :(");
		al_draw_text(size_32, al_map_rgb(200, 0, 30), SCREEN_W/3, SCREEN_H/2, 0, my_text);
	}

	
	//reinicializa a tela
	al_flip_display();	
    al_rest(2);			
	
  
   al_destroy_timer(timer);
   al_destroy_display(display);
   al_destroy_event_queue(event_queue);
 
   return 0;
}