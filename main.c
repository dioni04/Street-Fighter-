#include "street.h"

int main(){
    al_init();
    mustInit(al_init_primitives_addon(), "primitives");
    mustInit(al_init_font_addon(), "font");
    mustInit(al_init_ttf_addon(), "ttf font");
    mustInit(al_install_keyboard(), "keyboard");
    mustInit(al_install_mouse(), "mouse");
    // al_install_audio();
    // al_init_acodec_addon();
    mustInit(al_init_image_addon(), "image");
    // al_reserve_samples(16);

    ALLEGRO_TIMER* timer = al_create_timer(1.0 / FPS);
    mustInit(timer != NULL, "timer");
    ALLEGRO_TIMER* seconds = al_create_timer(1.0);
    mustInit(seconds != NULL, "seconds");
    ALLEGRO_TIMER* animationFrameTime = al_create_timer(GLOBAL_FRAME_TIME);
    mustInit(animationFrameTime != NULL, "animationFrameTime");
    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
    mustInit(queue != NULL, "queue");

    ALLEGRO_FONT* font = al_load_ttf_font("fonts/KnightVision-p7Ezy.ttf", MAX_Y*0.05, 0);
    mustInit(font != NULL, "font pointer");
    ALLEGRO_DISPLAY* disp = al_create_display(MAX_X, MAX_Y);
    mustInit(disp != NULL, "display");

    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_display_event_source(disp));
    al_register_event_source(queue, al_get_timer_event_source(timer));
    al_register_event_source(queue, al_get_timer_event_source(seconds));
    al_register_event_source(queue, al_get_timer_event_source(animationFrameTime));

    ALLEGRO_EVENT event;

    al_start_timer(timer);
    al_start_timer(seconds);
    al_start_timer(animationFrameTime);


    MATCH* match;
    PLAYER* player1;
    PLAYER* player2;

    /*LOOP PARTIDA*/
    match = createMatch(dojo);
    mustInit(match != NULL, "match");
    player1 = match->P1;
    player2 = match->P2;
   
    //Registra timers
    registerTimers(queue, player1);
    registerTimers(queue, player2);


    bool redraw = false;
    while(1){
        char counterStr[10];
        al_wait_for_event(queue, &event);

        if(event.type == ALLEGRO_EVENT_DISPLAY_CLOSE)//Botao 'X' da Janela
            break;
        if(event.type == ALLEGRO_EVENT_KEY_DOWN && event.keyboard.keycode == ALLEGRO_KEY_ESCAPE){
            short select = pauseMenu(queue, &event, font, match);
            if(select == exitGame)
                break;
            else if(select == mainMenu)
                break;//temp
        }
        keybinds(event, player1, player2);

        if(event.type == ALLEGRO_EVENT_TIMER){ //Novo clock event
            redraw = true;
            //COOLDOWNS
            cooldowns(event, player1);
            cooldowns(event, player2);
            animationSelect(player1);
            animationSelect(player2);
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
            else if(event.timer.source == animationFrameTime){
                if(player1->projs){
                    PROJECTILE* aux = player1->projs;
                    while(aux){
                        aux->currentFrame = (aux->currentFrame + 1) % player1->sizeSprites;
                        aux = aux->next;
                    }
                }
                if(player2->projs){
                    PROJECTILE* aux = player2->projs;
                    while(aux){
                        aux->currentFrame = (aux->currentFrame + 1) % player2->sizeSprites;
                        aux = aux->next;
                    }
                }
            }
            //Atualizacao frame de animacao
            else if(event.timer.source == player1->fighter.frameMovement || event.timer.source == player1->fighter.frameAttack)
                player1->fighter.currentFrame = (player1->fighter.currentFrame + 1) % player1->fighter.size;//proximo frame
            else if(event.timer.source == player2->fighter.frameMovement || event.timer.source == player2->fighter.frameAttack)
                player2->fighter.currentFrame = (player2->fighter.currentFrame + 1) % player2->fighter.size;//proximo frame
        }
        if(redraw && al_is_event_queue_empty(queue)){ //Novo Frame
            redraw = false;
            //CHECKS DE MOVEMENTO E ATAQUES
            movePlayer(match, player1, player2);
            movePlayer(match, player2, player1);
            moveProjectile(player1, player2);
            moveProjectile(player2, player1);
            checkHitAttack(player1, player2);
            checkHitAttack(player2, player1);

            al_clear_to_color(al_map_rgb(0,0,0));//black
            drawMap(match);
            //SPRITES
            drawShadow(player1);
            drawShadow(player2);
            drawCharacter(player1, player2);

            /*
            al_draw_filled_rectangle(player1->x - player1->length / 2, player1->y - player1->height / 2,player1->x + player1->length / 2, player1->y + player1->height / 2,al_map_rgb(0,0,255));
            al_draw_filled_rectangle(player2->x - player2->length / 2, player2->y - player2->height / 2,player2->x + player2->length / 2, player2->y + player2->height / 2,al_map_rgb(255,0,0));
            */
            //CONTADOR
            snprintf(counterStr, sizeof(counterStr), "%d",match->time);
            al_draw_text(font, al_map_rgb(255, 255, 255), MAX_X / 2, HEADER_LEVEL / 8, ALLEGRO_ALIGN_CENTER, counterStr);

            //PROJETEIS
            drawProjectile(player1);
            drawProjectile(player2);
            drawRounds(player1, player2);

            short result;
            result = drawUI(disp, font, queue, event, match, player1, player2);
            if(result == mainMenu)
                break;//temp
            al_flip_display();
        }
    }
    destroyMatch(match);
    al_destroy_timer(timer);
    al_destroy_timer(seconds);
    al_destroy_timer(animationFrameTime);
    al_destroy_event_queue(queue);
    al_destroy_font(font);
    al_destroy_display(disp);

    return 0;
}
