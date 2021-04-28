#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <math.h>


#define PI (3.141592653589793)


const float FPS = 60;
const int SCREEN_W = 640*2;
const int SCREEN_H = 480;


int main(int argc, char **argv){
   ALLEGRO_DISPLAY *display = NULL;
   ALLEGRO_EVENT_QUEUE *event_queue = NULL;
   ALLEGRO_TIMER *timer = NULL;
   ALLEGRO_BITMAP *bird = NULL;
   ALLEGRO_BITMAP *pig = NULL;
   ALLEGRO_BITMAP *explosion = NULL;

   float bird_x0, bird_y0, bird_x, bird_y, bird_h, bird_w;
   float pig_x, pig_y, pig_h, pig_w;
   bool redraw = true;
   float angle;// = 45.0 * PI / 180;
   float speed;// = 90;

//------------------------------ INITIALIZATION PROCEDURES ----------------------------
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

    al_install_keyboard();
// ----------------------------- CREATING THE BITMAPS -----------------------------------

   al_init_image_addon();
   bird = al_load_bitmap("Angry-Bird.jpg");
   pig = al_load_bitmap("pig2.jpg");
   explosion = al_load_bitmap("explosion2.jpg");
   if(!bird || !pig) {
      fprintf(stderr, "failed to create bitmaps!\n");
      al_destroy_display(display);
      al_destroy_timer(timer);
      return -1;
   }
   bird_h = al_get_bitmap_height(bird);
   bird_w = al_get_bitmap_width(bird);
   bird_x0 = 0;
   bird_y0 = SCREEN_H - bird_h;
   bird_x = 0;
   bird_y = SCREEN_H - bird_h;

   pig_h = al_get_bitmap_height(pig);
   pig_w = al_get_bitmap_width(pig);
   pig_x = bird_x + pig_w + 700; //rand()%SCREEN_W;
   pig_y = SCREEN_H - pig_h;

   al_draw_bitmap(bird, bird_x, bird_y, 0);
   al_draw_bitmap(pig, pig_x, pig_y, 0);

   al_set_target_bitmap(al_get_backbuffer(display));

   // ----------------- CREATING THE EVENT QUEUE ----------------------------------------

   event_queue = al_create_event_queue();
   if(!event_queue) {
      fprintf(stderr, "failed to create event_queue!\n");
      al_destroy_bitmap(bird);
      al_destroy_display(display);
      al_destroy_timer(timer);
      return -1;
   }
   al_register_event_source(event_queue, al_get_keyboard_event_source());

   // ------------------------ INPUTING THE SPEED AND ANGLE ----------------------------------

    char speed_text[20];
    char angle_text[20];
    int speed_input = 45;
    int angle_input = 45;

    al_init_font_addon();
    al_init_ttf_addon();

    ALLEGRO_FONT *size_32 = al_load_font("arial.ttf", 32, 1);

    bool quit = false;
    while(!quit) {

            sprintf(speed_text, "Velocidade: %d", speed_input);
            sprintf(angle_text, "Angulo: %d", angle_input);

            al_clear_to_color(al_map_rgb(0,0,0));
            al_draw_bitmap(bird, bird_x, bird_y, 0);
            al_draw_bitmap(pig, pig_x, pig_y, 0);
            al_draw_text(size_32, al_map_rgb(128, 200, 30), 10, 10, 0, speed_text);
            al_draw_text(size_32, al_map_rgb(128, 200, 30), 10, 50, 0, angle_text);

            al_flip_display();

        ALLEGRO_EVENT e;
        al_wait_for_event(event_queue, &e);

        if(e.type == ALLEGRO_EVENT_KEY_DOWN) {
            switch(e.keyboard.keycode) {
                case ALLEGRO_KEY_W:
                    angle_input += 1;
                break;

                case ALLEGRO_KEY_S:
                    angle_input -= 1;
                break;

                case ALLEGRO_KEY_A:
                    speed_input -= 1;
                break;

                case ALLEGRO_KEY_D:
                    speed_input += 1;
                break;

                case ALLEGRO_KEY_ENTER:
                    quit = true;
                break;
                }

            }
    }


    //al_rest(3);

    //variables I will use to shoot the bird
    angle = angle_input * PI / 180.0;
    speed = (float) speed_input;

    //---------------

   al_register_event_source(event_queue, al_get_display_event_source(display));

   al_register_event_source(event_queue, al_get_timer_event_source(timer));

   al_clear_to_color(al_map_rgb(0,0,0));

   al_flip_display();

   al_start_timer(timer);



   // ------------------------- SHOOTING THE BIRD ---------------------------------

   float bird_time = 0.0;
   while(bird_x < SCREEN_W && bird_y < SCREEN_H)
   {
       printf("\n-> %f %f %f\n",bird_x, pig_x, pig_w);
      ALLEGRO_EVENT ev;
      al_wait_for_event(event_queue, &ev);

      if(ev.type == ALLEGRO_EVENT_TIMER) {

         bird_time = al_get_timer_count(timer)/10.0;
         bird_x = bird_x0 + speed * cos(angle) * bird_time;
         bird_y = bird_y0 - (speed * sin(angle) * bird_time - (9.8 * pow(bird_time,2)/2));




         redraw = true;
         //bird_time += 0.1;
      }
      else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE  ) {
         break;
      }

      if(redraw && al_is_event_queue_empty(event_queue)) {
         redraw = false;

         al_clear_to_color(al_map_rgb(0,0,0));

         al_draw_bitmap(bird, bird_x, bird_y, 0);
         //draw pig
         al_draw_bitmap(pig, pig_x, pig_y, 0);

         al_flip_display();


      }
      //hit the pig
      if( (bird_x > pig_x && bird_x < pig_x + pig_w) && (bird_y < pig_y + pig_h && bird_y > pig_y) ) {
         //acertou
         printf("\n !! %f %f %f\n",bird_x, pig_x, pig_w);
         al_clear_to_color(al_map_rgb(0,0,0));
         al_draw_bitmap(explosion, pig_x, SCREEN_H - al_get_bitmap_height(explosion), 0);
         al_flip_display();
         al_rest(3);
         break;

      }
   }

   // -------------------- FINISHING THE GAME ------------------------------------------

   al_destroy_bitmap(pig);
   al_destroy_bitmap(bird);
   al_destroy_bitmap(explosion);
   al_destroy_timer(timer);
   al_destroy_display(display);
   al_destroy_event_queue(event_queue);

   return 0;
}
