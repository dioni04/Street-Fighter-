#include "street.h"
#include <allegro5/allegro_font.h>

int main(){
    al_init();
    al_init_primitives_addon();
    al_init_font_addon();
    al_init_ttf_addon();
    al_install_keyboard();
    al_install_audio();
    al_init_acodec_addon();
    al_init_image_addon();
    al_reserve_samples(16);

    ALLEGRO_TIMER* timer = al_create_timer(1.0 / FPS);
    if(!timer)
        return 1;
    ALLEGRO_TIMER* seconds = al_create_timer(1.0);
    if(!seconds)
        return 1;

    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
    if(!queue)
        return 1;
    ALLEGRO_FONT* font = al_load_ttf_font("fonts/KnightVision-p7Ezy.ttf", 20, 0);
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
    PROJECTILE* aux;

    /*LOOP PARTIDA*/
    match = createMatch();
    if(!match)
        return 1;
    player1 = match->P1;
    player2 = match->P2;
   
    al_register_event_source(queue, al_get_timer_event_source(player1->cooldownProj));
    al_register_event_source(queue, al_get_timer_event_source(player1->cooldownAttack));
    al_register_event_source(queue, al_get_timer_event_source(player2->cooldownProj));
    al_register_event_source(queue, al_get_timer_event_source(player2->cooldownAttack));
    al_register_event_source(queue, al_get_timer_event_source(player1->attackDuration));
    al_register_event_source(queue, al_get_timer_event_source(player2->attackDuration));
    al_register_event_source(queue, al_get_timer_event_source(player1->damageState));
    al_register_event_source(queue, al_get_timer_event_source(player2->damageState));
    bool redraw = true;

    while(1){
        char counterStr[10];
        al_wait_for_event(queue, &event);

        if(event.type == ALLEGRO_EVENT_DISPLAY_CLOSE || event.keyboard.keycode == ALLEGRO_KEY_ESCAPE)//Botao 'X' da Janela
            break;
        keybinds(event, player1, player2);

        if(event.type == ALLEGRO_EVENT_TIMER){ //Novo clock event
            redraw = true;
            //COOLDOWNS
            cooldowns(event, player1);
            cooldowns(event, player2);

            if(event.timer.source == seconds){
                //Timer a todo segundo
                match->time--;
                //Round Over
                if(!match->time){
                    if(player1->health == player2->health){
                        roundEnd(disp, font, match, player1, player2, queue, event);
                    }
                    else if(player1->health > player2->health){
                        player1->rounds++;
                        roundEnd(disp, font, match, player1, NULL, queue, event);
                        if(player1->rounds == 2)
                            break;
                    }
                    else{
                        player2->rounds++;
                        roundEnd(disp, font, match, player2, NULL, queue, event);
                        if(player2->rounds == 2)
                            break;
                    }
                }
            }
        }
        if(redraw && al_is_event_queue_empty(queue)){ //Novo Frame
            redraw = false;
            movePlayer(match, player1, player2);
            movePlayer(match, player2, player1);
            moveProjectile(player1, player2);
            moveProjectile(player2, player1);
            checkHitAttack(player1, player2);
            checkHitAttack(player2, player1);

            al_clear_to_color(al_map_rgb(0,0,0));//black
            for(int i = 1; i <= IMAGE_NUM; i++)
                al_draw_bitmap(match->map.map[i],0,0, 0);

            al_draw_filled_rectangle(player1->x - player1->length / 2, player1->y - player1->height / 2,player1->x + player1->length / 2, player1->y + player1->height / 2,al_map_rgb(0,0,255));
            al_draw_filled_rectangle(player2->x - player2->length / 2, player2->y - player2->height / 2,player2->x + player2->length / 2, player2->y + player2->height / 2,al_map_rgb(255,0,0));

            snprintf(counterStr, sizeof(counterStr), "%d",match->time);
            al_draw_text(font, al_map_rgb(255, 255, 255), MAX_X / 2, HEADER_LEVEL / 8, ALLEGRO_ALIGN_CENTER, counterStr);

            //PROJETEIS
            if(player1->projs){
                aux = player1->projs;
                while(aux){
                    al_draw_filled_rectangle(aux->x - aux->side / 2, aux->y - aux->side, aux->x + aux->side,player1->projs->y + aux->side, al_map_rgb(0,255,0));
                    aux = aux->next;
                }
            }
            if(player2->projs){
                aux = player2->projs;
                while(aux){
                    al_draw_filled_rectangle(aux->x - aux->side / 2, aux->y - aux->side, aux->x + aux->side,player2->projs->y + aux->side, al_map_rgb(255,0,0));
                    aux = aux->next;
                }
            }

            //VIDAS DOS JOGADORES

            if(player1->health > 0)
                al_draw_filled_rectangle((MAX_X * 0.454)*(1.0275 - player1->health*0.01),0, MAX_X*0.454, HEADER_LEVEL / 2,al_map_rgb(255, 0, 0)); //VIDA P1
            else{
                player2->rounds++;
                roundEnd(disp, font, match, player2, NULL, queue, event);
                if(player2->rounds == 2)
                    break;
            }

            if(player2->health > 0)
                al_draw_filled_rectangle(MAX_X*0.5475, 0 ,(MAX_X * 0.989) - ((1 - player2->health * 0.01) * MAX_X * 0.45), HEADER_LEVEL / 2, al_map_rgb(255, 0, 0)); //VIDA P2
            else{
                player1->rounds++;
                roundEnd(disp, font, match, player1, NULL, queue, event);
                if(player1->rounds == 2)
                    break;
            }
            //PLAYER1
            if(player1->rounds <= 1)
                al_draw_circle(MAX_X * 0.35,HEADER_LEVEL * 0.875, HEADER_LEVEL*0.125, al_map_rgb(255,255,255), -1);
            else  
                al_draw_filled_circle(MAX_X * 0.35,HEADER_LEVEL * 0.875, HEADER_LEVEL*0.125, al_map_rgb(255,255,255));
                //al_draw_filled_rectangle(MAX_X * 0.30,HEADER_LEVEL * 0.75, MAX_X* 0.35,HEADER_LEVEL,al_map_rgb(255,255,255));
            if(player1->rounds == 0)
                al_draw_circle(MAX_X * 0.40,HEADER_LEVEL * 0.875, HEADER_LEVEL * 0.125,al_map_rgb(255,255,255), -1);
            else
                al_draw_filled_circle(MAX_X * 0.40,HEADER_LEVEL * 0.875, HEADER_LEVEL * 0.125,al_map_rgb(255,255,255));
                ///al_draw_filled_rectangle(MAX_X * 0.40,HEADER_LEVEL * 0.75, MAX_X* 0.45,HEADER_LEVEL,al_map_rgb(255,255,255));
            //PLAYER2
            if(player2->rounds == 0)
                al_draw_circle(MAX_X * 0.60,HEADER_LEVEL * 0.875, HEADER_LEVEL * 0.125,al_map_rgb(255,255,255), -1);
                //al_draw_rectangle(MAX_X * 0.55,HEADER_LEVEL * 0.75, MAX_X* 0.60,HEADER_LEVEL,al_map_rgb(255,255,255), -1);
            else
                al_draw_filled_circle(MAX_X * 0.60,HEADER_LEVEL * 0.875, HEADER_LEVEL * 0.125,al_map_rgb(255,255,255));
                // al_draw_filled_rectangle(MAX_X * 0.55,HEADER_LEVEL * 0.75, MAX_X* 0.60,HEADER_LEVEL,al_map_rgb(255,255,255));
            if(player2->rounds <= 1)
                al_draw_circle(MAX_X * 0.65,HEADER_LEVEL * 0.875, HEADER_LEVEL * 0.125,al_map_rgb(255,255,255), -1);
                //al_draw_rectangle(MAX_X * 0.65,HEADER_LEVEL * 0.75, MAX_X* 0.70,HEADER_LEVEL,al_map_rgb(255,255,255), -1);
            else
                al_draw_filled_circle(MAX_X * 0.65,HEADER_LEVEL * 0.875, HEADER_LEVEL * 0.125,al_map_rgb(255,255,255));
                //al_draw_filled_rectangle(MAX_X * 0.65,HEADER_LEVEL * 0.75, MAX_X* 0.70,HEADER_LEVEL,al_map_rgb(255,255,255));
            //STAMINA
            staminaRegen(player1);
            staminaRegen(player2);

            if(player1->stamina > 0)
                al_draw_filled_rectangle((MAX_X * 0.25)*(1.06 - player1->stamina*0.01),HEADER_LEVEL * 0.75, MAX_X*0.31, HEADER_LEVEL,al_map_rgb(0, 255, 0)); //Stamina P1

            if(player2->stamina > 0)
                al_draw_filled_rectangle(MAX_X*0.695, HEADER_LEVEL * 0.75,(MAX_X * 0.989) - ((1 - player2->stamina * 0.01) * MAX_X * 0.25) ,HEADER_LEVEL,al_map_rgb(0, 255, 0)); //Stamina P2

            al_draw_bitmap(match->ui.healthBar,-5, 0, 0);
            al_draw_bitmap(match->ui.healthBar,MAX_X * 0.525, 0, 0);
            //BARRAS DE STAMINA
            al_draw_bitmap(match->ui.staminaBar,0, HEADER_LEVEL* 0.75, 0);
            al_draw_bitmap(match->ui.staminaBar,MAX_X * 0.68, HEADER_LEVEL* 0.75, 0);
            //al_draw_filled_rectangle(0, GROUND_LEVEL, MAX_X, MAX_Y,al_map_rgb(255, 255, 255));

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
