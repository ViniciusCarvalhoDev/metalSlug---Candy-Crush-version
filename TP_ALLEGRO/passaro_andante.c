#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>

//variaveis globais
const float FPS = 10; //numero de quadros por segundo, a ser carregado no temporizador (altere esse valor e veja o que acontece)
const int SCREEN_W = 640;  //largura da tela
const int SCREEN_H = 480;  //altura da tela

int main(int argc, char **argv){

    //tela
    ALLEGRO_DISPLAY *display = NULL;
    //fila de eventos detectados pelo Allegro (ex: tecla que foi apertada, clique do mouse etc)
    ALLEGRO_EVENT_QUEUE *event_queue = NULL;
    //temporizador: quando FPS = 10, a cada 0.1 segundos o tempo passa de t para t+1 e a fila de eventos detecta
    ALLEGRO_TIMER *timer = NULL;
    //figura do passaro
    ALLEGRO_BITMAP *bird = NULL;
    //variavel que indica se eh para redesenhar o passaro
    bool redraw = true;


    //------------------------------ rotinas de inicializacao -------------------------


	//inicializa o allegro. Se nao conseguir, imprime na tela uma msg de erro.
    if(!al_init()) {
      fprintf(stderr, "failed to initialize allegro!\n");
      return -1;
    }

	//inicializa o temporizador com a taxa de 1 quadro a cada 0.1 segundos. Se nao conseguir, imprime na tela uma msg de erro.
    timer = al_create_timer(1.0 / FPS);
    if(!timer) {
      fprintf(stderr, "failed to create timer!\n");
      return -1;
    }

	//inicializa a tela. Se nao conseguir, imprime na tela uma msg de erro.
    display = al_create_display(SCREEN_W, SCREEN_H);
    if(!display) {
      fprintf(stderr, "failed to create display!\n");
      al_destroy_timer(timer);
      return -1;
    }

    //inicializar o modulo de imagens
    al_init_image_addon();

    //carregar a figura .jpg na variavel bird
    bird = al_load_bitmap("Angry-Bird.jpg");
	//se nao conseguiu achar o arquivo, imprime uma msg de erro
    if(!bird) {
      fprintf(stderr, "failed to create bird bitmap!\n");
      al_destroy_display(display);
      al_destroy_timer(timer);
      return -1;
    }
    //largura e altura da figura do passaro
    float largura_passaro =  al_get_bitmap_width(bird);
    float altura_passaro =  al_get_bitmap_height(bird);
    //posicao x e y inicial do passaro na tela
    float bird_x = 0;
    float bird_y = SCREEN_H / 2.0 -  altura_passaro / 2.0;

    //variacao de x e y ao longo do tempo
    float bird_dx = 1.0, bird_dy = 0.0;

    //criar a fila de eventos
    event_queue = al_create_event_queue();
	//se nao conseguiu criar a fila de eventos
    if(!event_queue) {
      fprintf(stderr, "failed to create event_queue!\n");
      al_destroy_bitmap(bird);
      al_destroy_display(display);
      al_destroy_timer(timer);
      return -1;
    }

    //registrar mudancas na tela dentro da fila de eventos, isto e, sempre que a tela mudar, um evento ocorrerah
    al_register_event_source(event_queue, al_get_display_event_source(display));

    //coloca o timer na fila de eventos, isto e, sempre que o tempo passar, um evento eh gerado
    al_register_event_source(event_queue, al_get_timer_event_source(timer));

    //limpa a tela e coloca o fundo preto, cor: rgb(0,0,0) = preto
    al_clear_to_color(al_map_rgb(0,0,0));

    //reinicializa a tela
    al_flip_display();

    //inicia o temporizador
    al_start_timer(timer);

    //enquanto a posicao x do passaro for menor que a largura da tela
    while(bird_x < SCREEN_W)
    {
        //variavel do tipo evento
        ALLEGRO_EVENT ev;
        //a variavel ev recebe o primeiro evento da fila de eventos
        al_wait_for_event(event_queue, &ev);
        //se for um evento de timer, ou seja, se foi o tempo que passou de t para t+1
        if(ev.type == ALLEGRO_EVENT_TIMER) {
            //incrementa as posicoes x e y do passaro com o seu deslocamento dx e dy
            bird_x += bird_dx;
            bird_y += bird_dy;
            //como eu movi o passaro, preciso redesenhar ele (remova essa linha e veja o que acontece)
            redraw = true;
        }
        //se o evento for o fechamento da tela (clicando no x no canto superior direito)
        else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE  ) {
            //paro de executar o while
            break;
        }
        //se for para eu redesenhar a tela e nao tenho mais nenhum evento para ler
        if(redraw && al_is_event_queue_empty(event_queue)) {
            //nao preciso redesenhar
            redraw = false;
            //limpo a tela
            al_clear_to_color(al_map_rgb(0,0,0));
            //desenho o passaro na nova posicao
            al_draw_bitmap(bird, bird_x, bird_y, 0);
            //dou um refresh na tela
            al_flip_display();
        }
    } //fim while

    //rotinas de fim de jogo
    al_destroy_bitmap(bird);
    al_destroy_timer(timer);
    al_destroy_display(display);
    al_destroy_event_queue(event_queue);

    return 0;
}
