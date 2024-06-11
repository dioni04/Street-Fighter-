#include "street.h"
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/color.h>
#include <allegro5/events.h>
#include <allegro5/keycodes.h>
#include <allegro5/timer.h>
#include <stdio.h>

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
    /*player1->cooldownProj = al_create_timer(PROJ_COOLDOWN);
    if(!player1->cooldownProj)
        return 1;
    player1->cooldownAttack = al_create_timer(ATTACK_COOLDOWN);
    if(!player1->cooldownAttack)
        return 1;
    player1->attackDuration = al_create_timer(ATTACK_DURATION);
    if(!player1->attackDuration)
        return 1;
    player2->cooldownProj = al_create_timer(PROJ_COOLDOWN);
    if(!player2->cooldownProj)
        return 1;
    player2->cooldownAttack = al_create_timer(ATTACK_COOLDOWN);
    if(!player2->cooldownAttack)
        return 1;
    player2->attackDuration = al_create_timer(ATTACK_DURATION);
    if(!player2->attackDuration)
        return 1;
    */
    //printf("%f\n", player1->speedX);
    al_register_event_source(queue, al_get_timer_event_source(player1->cooldownProj));
    al_register_event_source(queue, al_get_timer_event_source(player1->cooldownAttack));
    al_register_event_source(queue, al_get_timer_event_source(player2->cooldownProj));
    al_register_event_source(queue, al_get_timer_event_source(player2->cooldownAttack));
    al_register_event_source(queue, al_get_timer_event_source(player1->attackDuration));
    al_register_event_source(queue, al_get_timer_event_source(player2->attackDuration));
 
    bool redraw = true;

    while(1){
        char counterStr[10];
        al_wait_for_event(queue, &event);

        if(event.type == ALLEGRO_EVENT_DISPLAY_CLOSE || event.keyboard.keycode == ALLEGRO_KEY_ESCAPE)//Botao 'X' da Janela
            break;
        if(event.type == ALLEGRO_EVENT_KEY_DOWN){//Keybinds dos ataques
            if(event.keyboard.keycode == ALLEGRO_KEY_Y && !al_get_timer_started(player1->cooldownProj) && player1->state == stand && player1->stamina >= PROJ_COST){
                if(AT_LEFT(player1->x, player2->x))
                    player1->projs = createProjectile(player1, right, player1->projs);
                else
                    player1->projs = createProjectile(player1, left, player1->projs);
                // printf("R");
                al_start_timer(player1->cooldownProj);
                player1->stamina -= PROJ_COST;
            }
            if(event.keyboard.keycode == ALLEGRO_KEY_L && !al_get_timer_started(player2->cooldownProj) && player2->state == stand && player2->stamina >= PROJ_COST){
                if(AT_LEFT(player2->x, player1->x))
                    player2->projs = createProjectile(player2, right, player2->projs);
                else
                    player2->projs = createProjectile(player2, left, player2->projs);
                // printf("R");
                al_start_timer(player2->cooldownProj);
                player2->stamina -= PROJ_COST;    
            }
      /*
            if(event.keyboard.keycode == ALLEGRO_KEY_T && player1->state == stand && player1->cooldownAttack <= 0){
                if(AT_LEFT(player1->x, player2->x))
                    createAttack(player1, punch,left);
                else
                    createAttack(player1, punch,right);
                al_start_timer(player1->cooldownAttack);
            }
            */
        }
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
        }
        if(event.type == ALLEGRO_EVENT_TIMER){ //Novo clock event
            redraw = true;
            //COOLDOWNS
            if(event.timer.source == player1->cooldownAttack)
                al_stop_timer(player1->cooldownAttack);
            if(event.timer.source == player1->cooldownProj)
                al_stop_timer(player1->cooldownProj);
            if(event.timer.source == player1->attackDuration)
                al_stop_timer(player1->attackDuration);
            if(event.timer.source == player2->cooldownAttack)
                al_stop_timer(player2->cooldownAttack);
            if(event.timer.source == player2->cooldownProj)
                al_stop_timer(player2->cooldownProj);
            if(event.timer.source == player2->attackDuration)
                al_stop_timer(player2->attackDuration);

            if(event.timer.source == seconds){
                //Timer a todo segundo
                match->time--;
                //Round Over
                if(!match->time){
                    if(player1->health == player2->health){
                        roundEnd(disp, font, match, player1, player2);
                    }
                    else if(player1->health > player2->health){
                        player1->rounds++;
                        if(player1->rounds == 2)
                            break;
                    }
                    else{
                        player2->rounds++;
                        roundEnd(disp, font, match, player2, NULL);
                        if(player2->rounds == 2)
                            break;
                    }
                    pressSpace(queue, &event);
                }
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

            player1->stamina += STAMINA_REGEN;
            player1->stamina = (player1->stamina > BASE_STAMINA) ? BASE_STAMINA : player1->stamina;//Limite superior
            if(player1->stamina > 0)
                al_draw_filled_rectangle((MAX_X * 0.25)*(1 - player1->stamina*0.01),HEADER_LEVEL * 0.75, MAX_X*0.25, HEADER_LEVEL,al_map_rgb(255, 0, 255)); //Stamina P1
            if(player1->projs){
                PROJECTILE* aux = player1->projs;
                while(aux){
                    al_draw_filled_rectangle(aux->x - aux->side / 2, aux->y - aux->side, aux->x + aux->side,player1->projs->y + aux->side, al_map_rgb(0,255,0));
                    aux = aux->next;
                }
            }
            if(player1->health > 0)
                al_draw_filled_rectangle((MAX_X * 0.45)*(1 - player1->health*0.01),0, MAX_X*0.45, HEADER_LEVEL / 2,al_map_rgb(255, 255, 255)); //VIDA P1
            else
                break;
            if(player2->health > 0)
                al_draw_filled_rectangle(MAX_X*0.55, 0 ,(MAX_X) - ((1 - player2->health * 0.01) * MAX_X * 0.45), HEADER_LEVEL / 2, al_map_rgb(255, 255, 255)); //VIDA P2
            else{
                player1->rounds++;
                roundEnd(disp, font, match, player1, NULL);
                while(1){//Tela fim de Round
                    al_wait_for_event(queue, &event);
                    if(event.type == ALLEGRO_EVENT_KEY_UP && event.keyboard.keycode == ALLEGRO_KEY_SPACE)
                        break;
                }
                if(player1->rounds == 2)
                    break;
            }
            if(player2->stamina > 0)
                al_draw_filled_rectangle(MAX_X*0.75, HEADER_LEVEL * 0.75,(MAX_X) - ((1 - player2->stamina * 0.01) * MAX_X * 0.25) ,HEADER_LEVEL,al_map_rgb(255, 0, 255)); //Stamina P2
            if(player1->state != crouch)
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
