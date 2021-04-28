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

int TILE_R = 50;
float TILE_SPEED = 1;
int TILE_HIT_H;
int TOTAL_TILES;

int LANE_W;
int NUM_LANES;

int TRACK_W;
int TRACK_LEFT_X;
int TRACK_RIGHT_X;
int KEY1 = ALLEGRO_KEY_1;

ALLEGRO_FONT *SIZE_TITLE;   
ALLEGRO_COLOR COLORS[MAXLANES];


int score = 0;
int count_tiles_created = 0;
int count_tiles_done = 0;

typedef struct Tile {
	float x,y; //x center, y bottom 
	int lane; 
	int status;
	int delay;
	int id;
	ALLEGRO_COLOR color;
} Tile;


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


void init_track() {
	int i;
	SKY_H = SCREEN_H * 0.25;
	LANE_W = (2*TILE_R) * 1.5;
	TILE_HIT_H = SCREEN_H - (2*TILE_R) * 1.2;
	TOTAL_TILES = AVG_TILES_PER_LANE*NUM_LANES;
	TRACK_W = NUM_LANES * LANE_W;
	TRACK_LEFT_X = SCREEN_W/2 - TRACK_W/2;
	TRACK_RIGHT_X = TRACK_LEFT_X + TRACK_W;
	
	SIZE_TITLE = al_load_font("arial.ttf", 64, 1);  
	for(i=0; i<NUM_LANES; i++) {
		COLORS[i] = al_map_rgb(30+rand()%226,30+rand()%226,30+rand()%226);
	}
}

void draw_scenario(ALLEGRO_DISPLAY *display) {
	
	int i;
	
	//screen
	ALLEGRO_COLOR BKG_COLOR = al_map_rgb(0,0,0); 
	al_set_target_bitmap(al_get_backbuffer(display));
	al_clear_to_color(BKG_COLOR);   
	
	//sky
	al_draw_filled_rectangle(0, 0, SCREEN_W, SKY_H, al_map_rgb(100,100,100));
	//TITLE
	//sprintf(texto, "\\a HERO");
    al_draw_text(SIZE_TITLE, al_map_rgb(255, 0, 0), SCREEN_W/2 - 150, SKY_H/4, 0, "\\a HERO");
   
   //desenha a pista:
   al_draw_line(TRACK_LEFT_X, SKY_H, TRACK_LEFT_X, SCREEN_H, al_map_rgb(255,255,255), 10); 
   al_draw_line(TRACK_RIGHT_X, SKY_H, TRACK_RIGHT_X, SCREEN_H, al_map_rgb(255,255,255), 10); 
   for(i=1; i<NUM_LANES; i++) {
	   al_draw_line(TRACK_LEFT_X+(LANE_W*i), SKY_H, TRACK_LEFT_X+(LANE_W*i), SCREEN_H, al_map_rgb(255,255,255), 3); 
   }
   
   //hit line
   al_draw_line(TRACK_LEFT_X, TILE_HIT_H, TRACK_RIGHT_X, TILE_HIT_H, al_map_rgb(255,0,0), 1); 
  
}

int tileDelayCollision(Tile t1, Tile t2) {
	float deltat = 2*TILE_R/(TILE_SPEED/5.0);
	if(abs(t1.delay - t2.delay) < deltat) {
		//printf("\ndelay collision: %d %d %f", t1.delay, t2.delay, deltat);
		return 1;
	}
	return 0;
}

int tilePositionCollision(Tile t1, Tile t2) {
	if(t1.status == ACTIVE) {
		float y_t1 = t2.delay + t1.y;
		if(y_t1 - TILE_R < t2.y + TILE_R)
			return 1;
	}
	return 0;
}

int tileCollision(Tile tiles[], Tile tile){
	int i;
	for(i=0; i<MAXTILES; i++) {
		if((tiles[i].status == CREATED || tiles[i].status == ACTIVE) &&
		   tiles[i].lane == tile.lane && 
		   tiles[i].id != tile.id &&
		   (tileDelayCollision(tiles[i], tile) ||
		    tilePositionCollision(tiles[i], tile))) {
			return 1;
		}
	}
	return 0;
	
}

void createRandomTile(Tile tiles[], int idx) {
	
	tiles[idx].y = SKY_H + (2*TILE_R);
	tiles[idx].status = LIMBO;
	//printf("\ncreated tile %d (%f)", idx, tiles[idx].y);	
	if(count_tiles_created < TOTAL_TILES) {
		tiles[idx].lane = rand()%NUM_LANES;
		tiles[idx].x = TRACK_LEFT_X + LANE_W*tiles[idx].lane + LANE_W/2.0;	tiles[idx].color = COLORS[tiles[idx].lane]; 		
		tiles[idx].status = CREATED;		
		do{
			//tiles[idx].delay = rand()%(TILE_R*(5 +(int)(5*log2(NUM_LANES)))*MAXTILES/NUM_LANES);
			tiles[idx].delay = rand()%((int)((5/(int)TILE_SPEED)*(MAXTILES*8*TILE_R)/(1+log2(NUM_LANES))));
			//printf("\n%d: %d", tiles[idx].id, tiles[idx].delay);
		} while(tileCollision(tiles, tiles[idx]));	
		count_tiles_created++;
	}
}

void initTiles(Tile tiles[]) {
	int i;
	for(i=0; i<MAXTILES; i++) {
		tiles[i].status = LIMBO;
		tiles[i].id = i;
	}
	for(i=0; i<MAXTILES; i++) {
		createRandomTile(tiles, i);
	}	
}

void updateTiles(Tile tiles[]) {
	int i;	
	for(i=0; i<MAXTILES; i++) {
		if(tiles[i].status!=CREATED && tiles[i].status!=LIMBO && tiles[i].y - TILE_R > SCREEN_H) {
			if(tiles[i].status == ACTIVE) {
				score -= TILE_SPEED*NUM_LANES;
				count_tiles_done++;
			}
			//printf("\n->count_tiles_done = %d (%f, %d)", count_tiles_done, tiles[i].y, i);			
			createRandomTile(tiles, i);
			
		}
		else if(tiles[i].status == CREATED && tiles[i].delay <= 0)
			tiles[i].status = ACTIVE;
		else if(tiles[i].status != CREATED){
			tiles[i].y += TILE_SPEED/5.0;
		}
		else {
			tiles[i].delay -= 1;
		}
	}	
}

void drawTiles(Tile tiles[]) {
	int i;	
	for(i=0; i<MAXTILES; i++) {
		if(tiles[i].y - TILE_R > SKY_H && tiles[i].status == ACTIVE) {
			al_draw_filled_circle(tiles[i].x, tiles[i].y, TILE_R, tiles[i].color);
		}
	}	
}

int tileInHitArea(Tile tile) {
	if(tile.y - TILE_R >= TILE_HIT_H && tile.y + TILE_R < SCREEN_H)
		return 1;
	return 0;	
}

int laneHit(int lane, Tile tiles[]) {
	int i;
	for(i=0; i<MAXTILES; i++) {
		if(tiles[i].lane == lane && 
		   tiles[i].status == ACTIVE &&
		   tileInHitArea(tiles[i])) {	
			tiles[i].status = HIT;
			//printf("\nlane %d, tile %d", lane, i);
			return 1;
		}
	}
	return 0;
	
}

void drawError(int lane) {
	al_draw_circle(TRACK_LEFT_X + LANE_W*lane + LANE_W/2.0, TILE_HIT_H + (SCREEN_H-TILE_HIT_H)/2.0, TILE_R, COLORS[lane], 4);
	//al_draw_filled_circle(TRACK_LEFT_X + LANE_W*lane + LANE_W/2.0, TILE_HIT_H + (SCREEN_H-TILE_HIT_H)/2.0, TILE_R, COLORS[lane]);	
	//al_rest(1);
}

int main(int argc, char **argv){
	
	srand(time(NULL));
	//srand(3345);
	ALLEGRO_DISPLAY *display = NULL;
	ALLEGRO_EVENT_QUEUE *event_queue = NULL;
	ALLEGRO_TIMER *timer = NULL;
	int i=0, miss=0;

	NUM_LANES = 3;
	TILE_SPEED = 3;
	
	if(argc > 1) {
		int nlanes = atoi(argv[1]);
		//printf("\nnlanes = %d", nlanes);
		if(nlanes>0 && nlanes<7) NUM_LANES = nlanes;
	}

	if(argc > 2) {
		int speed = atoi(argv[2]);
		//printf("\nspeed = %d", speed);
		if(speed>0 && speed<6) TILE_SPEED = speed;
	}


	
	Tile tiles[MAXTILES];
	
	int keys[MAXLANES];
	int errors[MAXLANES];
	for(i=0; i<MAXLANES; i++) {
		keys[i] = KEY1+i;
		errors[i] = 0;
	}
	

	
	char my_score[10];
	char texto[100];
 
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
	initTiles(tiles);
	
   int playing = 1;
	//enquanto playing for verdadeiro, faca:
   while(playing) {
      ALLEGRO_EVENT ev;
	  //espera por um evento e o armazena na variavel de evento ev
      al_wait_for_event(event_queue, &ev);
	  
	if(ev.type == ALLEGRO_EVENT_KEY_DOWN) {
		printf("\n%d", ev.keyboard.keycode);
		//verifica qual tecla foi
		
		for(i=0; i<MAXLANES; i++) {
			if(ev.keyboard.keycode == keys[i]) {
				if(laneHit(i, tiles)) {
					score+= TILE_SPEED*NUM_LANES;
					count_tiles_done++;
					printf("\a");
				}
				else {
					score-= TILE_SPEED;
					errors[i] = 1;
				}
			i = MAXLANES;
			}
		}
		
		
		if(ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
			playing = 0;
		}
 
   }
	//se o tipo de evento for um evento do temporizador, ou seja, se o tempo passou de t para t+1
    else if(ev.type == ALLEGRO_EVENT_TIMER) {
		draw_scenario(display);
		updateTiles(tiles);
		drawTiles(tiles);
		miss=0;
		for(i=0; i<MAXLANES; i++) {
			if(errors[i]) {
				drawError(i);
				errors[i] = 0;
				miss++;
			}
		}

		
		
		//SCORE
		sprintf(my_score, "%d", score);
        al_draw_text(size_32, al_map_rgb(255, 0, 0), SCREEN_W-100, 20, 0, my_score);
		

		//reinicializo a tela
		 al_flip_display();
		 if(miss > 0) {
			al_stop_timer(timer);
			al_rest(0.2);
			al_start_timer(timer);
		}
			 

    }
	//se o tipo de evento for o fechamento da tela (clique no x da janela)
	else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
		score = 0;
		playing = 0;
	}
	
	if(score < MIN_SCORE || count_tiles_done == TOTAL_TILES) {
		//printf("\ncount_tiles_done = %d", count_tiles_done);
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