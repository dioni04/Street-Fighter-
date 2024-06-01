#include "street.h"

int main(){

    GAME game;

    al_init();
    al_install_keyboard();

    ALLEGRO_TIMER* timer = al_create_timer(1.0 / FPS);
    if(!timer)
        return 1;
    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
    if(!queue)
        return 1;
    ALLEGRO_FONT* font = al_create_builtin_font();
    if(!font)
        return 1;
    ALLEGRO_DISPLAY* disp = al_create_display(MAX_X, MAX_Y);
    if(!disp)
        return 1;

    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_display_event_source(disp));
    al_register_event_source(queue, al_get_timer_event_source(timer));

    ALLEGRO_EVENT event;
    al_start_timer(timer);

    bool redraw = true;
    while(1){
        al_wait_for_event(queue, &event);

        if(event.type == 10 || event.type == 12){
            if(event.keyboard.keycode == 1){

            }
        }
        if(event.type == 42)//Botao 'X' da Janela
            break;
        if(event.type == 30) //Novo Frame
    }

    al_destroy_timer(timer);
    al_destroy_event_queue(queue);
    al_destroy_font(font);
    al_destroy_display(disp);

    return 0;
}
