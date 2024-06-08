#include "street.h"
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/color.h>

int main(){
    al_init();
    al_install_keyboard();

    ALLEGRO_TIMER* timer = al_create_timer(1.0 / FPS);
    if(!timer)
        return 1;
    ALLEGRO_TIMER* seconds = al_create_timer(1.0);
    if(!seconds)
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
    al_register_event_source(queue, al_get_timer_event_source(seconds));

    ALLEGRO_EVENT event;

    al_start_timer(timer);
    al_start_timer(seconds);

    MATCH* match;
    PLAYER* player1;
    PLAYER* player2;

    /*LOOP PARTIDA*/
    match = createMatch();
    if(!match)
        return 1;
    player1 = match->P1;
    player2 = match->P2;
    //printf("%f\n", player1->speedX);

    bool redraw = true;

    while(1){
        char counterStr[10];
        al_wait_for_event(queue, &event);

        //TECLAS DE MOVIMENTACAO
        if(event.type == ALLEGRO_EVENT_KEY_DOWN || event.type == ALLEGRO_EVENT_KEY_UP){
            if(event.keyboard.keycode == ALLEGRO_KEY_W){
                moveUp(player1);
            }
            if(event.keyboard.keycode == ALLEGRO_KEY_A){
                moveLeft(player1, player2);
            }
            if(event.keyboard.keycode == ALLEGRO_KEY_S){
                moveDown(player1, player2);
            }
            if(event.keyboard.keycode == ALLEGRO_KEY_D){
                moveRight(player1, player2);
            }
            if(event.keyboard.keycode == ALLEGRO_KEY_UP){
                moveUp(player2);
            }
            if(event.keyboard.keycode == ALLEGRO_KEY_LEFT){
                moveLeft(player2, player1);
            }
            if(event.keyboard.keycode == ALLEGRO_KEY_DOWN){
                moveDown(player2, player1);
            }
            if(event.keyboard.keycode == ALLEGRO_KEY_RIGHT){
                moveRight(player2, player1);
            }
            if(event.keyboard.keycode == ALLEGRO_KEY_R){
                if(AT_LEFT(player1->x, player2->x))
                    createProjectile(player1, left, player1->projs);
                else
                    createProjectile(player1, right, player1->projs);
                printf("R");
            }
            /* FAZER KEYBINDS DOS ATAQUES AQUI */
        }
        if(event.type == ALLEGRO_EVENT_DISPLAY_CLOSE || event.keyboard.keycode == ALLEGRO_KEY_ESCAPE)//Botao 'X' da Janela
            break;
        if(event.type == ALLEGRO_EVENT_TIMER){ //Novo clock event
            redraw = true;
            if(event.timer.source == seconds){
                //Timer a todo segundo
                match->time--;
                printf("%d\n", match->time);
                if(!match->time)
                    break;//END ROUND
            }
        }
        if(redraw && al_is_event_queue_empty(queue)){ //Novo Frame
            redraw = false;
            movePlayer(match, player1, player2);
            movePlayer(match, player2, player1);
            moveProjectile(player1, player2);
            moveProjectile(player2, player1);
            al_clear_to_color(al_map_rgb(0,0,0));
            snprintf(counterStr, sizeof(counterStr), "%d",match->time);
            al_draw_text(font, al_map_rgb(255, 255, 255), MAX_X / 2, HEADER_LEVEL / 4, ALLEGRO_ALIGN_CENTER, counterStr);
            if(player1->health > 0)
                al_draw_filled_rectangle((MAX_X * 0.45)*(1 - player1->health*0.01),0, MAX_X*0.45, HEADER_LEVEL / 2,al_map_rgb(255, 255, 255)); //VIDA P1
            if(player1->stamina > 0)
                al_draw_filled_rectangle((MAX_X * 0.25)*(1 - player1->stamina*0.01),HEADER_LEVEL * 0.75, MAX_X*0.25, HEADER_LEVEL,al_map_rgb(255, 0, 255)); //Stamina P1
            if(player1->projs)
                al_draw_filled_rectangle(player1->projs->x - player1->projs->side / 2, player1->projs->y - player1->projs->side, player1->projs->x + player1->projs->side,player1->projs->y + player1->projs->side, al_map_rgb(0,255,0));
            if(player2->health > 0)
                al_draw_filled_rectangle(MAX_X*0.55, 0 ,(MAX_X) - ((1 - player2->health * 0.01) * MAX_X * 0.45), HEADER_LEVEL / 2, al_map_rgb(255, 255, 255)); //VIDA P2
            if(player2->stamina > 0)
                al_draw_filled_rectangle(MAX_X*0.75, HEADER_LEVEL * 0.75,(MAX_X) - ((1 - player2->stamina * 0.01) * MAX_X * 0.25) ,HEADER_LEVEL,al_map_rgb(255, 0, 255)); //Stamina P2
            al_draw_filled_rectangle(player1->x - player1->length / 2, player1->y - player1->height / 2,player1->x + player1->length / 2, player1->y + player1->height / 2,al_map_rgb(0,0,255));
            al_draw_filled_rectangle(player2->x - player2->length / 2, player2->y - player2->height / 2,player2->x + player2->length / 2, player2->y + player2->height / 2,al_map_rgb(255,0,0));
            al_draw_filled_rectangle(0, GROUND_LEVEL, MAX_X, MAX_Y,al_map_rgb(255, 255, 255));
            al_flip_display();
        }
    }

    destroyMatch(match);
    al_destroy_timer(timer);
    al_destroy_timer(seconds);
    al_destroy_event_queue(queue);
    al_destroy_font(font);
    al_destroy_display(disp);

    return 0;
}
