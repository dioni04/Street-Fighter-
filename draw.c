#include "street.h"

void drawMap(MATCH* match){
    //MAPA
    for(int i = 0; i < match->map.size; i++){
                float ratioX = MAX_X / al_get_bitmap_width(match->map.map[i]);
                float ratioY = MAX_Y / al_get_bitmap_height(match->map.map[i]);
                al_draw_scaled_bitmap(match->map.map[i],0,0,
                                      al_get_bitmap_width(match->map.map[i]),
                                      al_get_bitmap_height(match->map.map[i]),
                                      0,0,
                                      al_get_bitmap_width(match->map.map[i]) * ratioX,
                                      al_get_bitmap_height(match->map.map[i]) * ratioY,
                                      0);
            }
    return;
}

void drawShadow(PLAYER* player){
    if(player->directionY != none)
        return;
    ALLEGRO_BITMAP* shadow = al_load_bitmap("images/fighters/shadow.png");
    float ratio = (MAX_X * 0.18) / al_get_bitmap_width(shadow);
    al_draw_scaled_bitmap(shadow,0,0,al_get_bitmap_width(shadow),al_get_bitmap_height(shadow),player->x - player->length,player->y + player->height*0.4,al_get_bitmap_width(shadow) * ratio,al_get_bitmap_height(shadow)* ratio,0);
    al_destroy_bitmap(shadow);
    return;
}

void drawPlayer(PLAYER* player, float ratio, short flag){
    if(al_get_timer_started(player->damageState))
        al_draw_tinted_scaled_bitmap(player->fighter.sprite[player->fighter.currentFrame],
                                        al_map_rgb(255, 0, 0),0,0,
                                        al_get_bitmap_width(player->fighter.sprite[player->fighter.currentFrame]),
                                        al_get_bitmap_height(player->fighter.sprite[player->fighter.currentFrame]),
                                        player->x - player->length * 2, player->y - player->height,
                                        al_get_bitmap_width(player->fighter.sprite[player->fighter.currentFrame]) * ratio,
                                        al_get_bitmap_height(player->fighter.sprite[player->fighter.currentFrame]) * ratio,
                                        flag);
    else
        al_draw_scaled_bitmap(player->fighter.sprite[player->fighter.currentFrame],
                                        0,0,
                                        al_get_bitmap_width(player->fighter.sprite[player->fighter.currentFrame]),
                                        al_get_bitmap_height(player->fighter.sprite[player->fighter.currentFrame]),
                                        player->x - player->length * 2, player->y - player->height,
                                        al_get_bitmap_width(player->fighter.sprite[player->fighter.currentFrame]) * ratio,
                                        al_get_bitmap_height(player->fighter.sprite[player->fighter.currentFrame]) * ratio,
                                        flag);
    return;
}

void drawCharacter(PLAYER* player1, PLAYER* player2){
    float ratio = (MAX_X * 0.375) / al_get_bitmap_width(player1->fighter.sprite[player1->fighter.currentFrame]);
    if(AT_LEFT(player1->x, player2->x)){
        drawPlayer(player1, ratio, 0);
        drawPlayer(player2, ratio, ALLEGRO_FLIP_HORIZONTAL);
                // al_draw_bitmap(player2->fighter.sprite[player2->fighter.currentFrame],player2->x - player2->length / 2, player2->y - player2->height / 2 , ALLEGRO_FLIP_HORIZONTAL);
    }
    else{
        drawPlayer(player1, ratio, ALLEGRO_FLIP_HORIZONTAL);
        drawPlayer(player2, ratio, 0);
    }
    return;
}

void drawProjectile(PLAYER* player){
    if(player->projs){
        float ratio = (MAX_X * 0.10) / al_get_bitmap_width(player->spritesProjs[player->projs->currentFrame]);
        PROJECTILE* aux = player->projs;
        while(aux){
            if(aux->direction == right)
            //al_draw_filled_rectangle(aux->x - aux->side / 2, aux->y - aux->side, aux->x + aux->side,player1->projs->y + aux->side, al_map_rgb(0,255,0));
                al_draw_scaled_bitmap(player->spritesProjs[aux->currentFrame],
                                        0,0,
                                        al_get_bitmap_width(player->spritesProjs[aux->currentFrame]),
                                        al_get_bitmap_height(player->spritesProjs[aux->currentFrame]),
                                        aux->x , aux->y - aux->side,
                                        al_get_bitmap_width(player->spritesProjs[aux->currentFrame]) * ratio,
                                        al_get_bitmap_height(player->spritesProjs[aux->currentFrame]) * ratio,
                                        0);
            else
                al_draw_scaled_bitmap(player->spritesProjs[aux->currentFrame],
                                        0,0,
                                        al_get_bitmap_width(player->spritesProjs[aux->currentFrame]),
                                        al_get_bitmap_height(player->spritesProjs[aux->currentFrame]),
                                        aux->x , aux->y - aux->side,
                                        al_get_bitmap_width(player->spritesProjs[aux->currentFrame]) * ratio,
                                        al_get_bitmap_height(player->spritesProjs[aux->currentFrame]) * ratio,
                                        ALLEGRO_FLIP_HORIZONTAL);
            aux = aux->next;
        }
    }
    return;
}

short drawUI(ALLEGRO_DISPLAY* disp, ALLEGRO_FONT* font, ALLEGRO_EVENT_QUEUE* queue, ALLEGRO_EVENT event ,MATCH* match,PLAYER* player1, PLAYER* player2){
    if(player1->health > 0)
        al_draw_filled_rounded_rectangle((MAX_X * 0.454)*(1.0275 - player1->health*0.01),0, MAX_X*0.454, HEADER_LEVEL / 2, 10, 15,al_map_rgb(255, 0, 0)); //VIDA P1
    else{
        player2->rounds++;
        roundEnd(disp, font, match, player2, NULL, queue, event);
        if(player2->rounds == 2)
            return mainMenu;
    }

    if(player2->health > 0)
        al_draw_filled_rounded_rectangle(MAX_X*0.5475, 0 ,(MAX_X * 0.989) - ((1 - player2->health * 0.01) * MAX_X * 0.45), HEADER_LEVEL / 2, 10, 15,al_map_rgb(255, 0, 0)); //VIDA P2
    else{
        player1->rounds++;
        roundEnd(disp, font, match, player1, NULL, queue, event);
        if(player1->rounds == 2)
            return mainMenu;
    }
    //STAMINA
    staminaRegen(player1);
    staminaRegen(player2);

    if(player1->stamina > 0)
        al_draw_filled_rectangle((MAX_X * 0.25)*(1.06 - player1->stamina*0.01),HEADER_LEVEL * 0.75, MAX_X*0.31, HEADER_LEVEL,al_map_rgb(0, 255, 0)); //Stamina P1
    if(player2->stamina > 0)
        al_draw_filled_rectangle(MAX_X*0.695, HEADER_LEVEL * 0.75,(MAX_X * 0.989) - ((1 - player2->stamina * 0.01) * MAX_X * 0.25) ,HEADER_LEVEL,al_map_rgb(0, 255, 0)); //Stamina P2

    //BARRAS DE VIDA
    float ratio = (MAX_X*0.48) / al_get_bitmap_width(match->ui.healthBar);
    al_draw_scaled_bitmap(match->ui.healthBar, 0, 0, al_get_bitmap_width(match->ui.healthBar), al_get_bitmap_height(match->ui.healthBar),
                            -10, 0, al_get_bitmap_width(match->ui.healthBar) * ratio, al_get_bitmap_height(match->ui.healthBar) * ratio, 0);
    al_draw_scaled_bitmap(match->ui.healthBar, 0, 0, al_get_bitmap_width(match->ui.healthBar), al_get_bitmap_height(match->ui.healthBar),
                            MAX_X * 0.525, 0, al_get_bitmap_width(match->ui.healthBar) * ratio, al_get_bitmap_height(match->ui.healthBar) * ratio, 0);
    //BARRAS DE STAMINA
    ratio = (MAX_X*0.32) / al_get_bitmap_width(match->ui.healthBar);
    al_draw_scaled_bitmap(match->ui.staminaBar, 0, 0, al_get_bitmap_width(match->ui.staminaBar), al_get_bitmap_height(match->ui.staminaBar),
                            0, HEADER_LEVEL * 0.7, al_get_bitmap_width(match->ui.staminaBar) * ratio, al_get_bitmap_height(match->ui.staminaBar) * ratio, 0);
    al_draw_scaled_bitmap(match->ui.staminaBar, 0, 0, al_get_bitmap_width(match->ui.staminaBar), al_get_bitmap_height(match->ui.staminaBar),
                            MAX_X*0.68, HEADER_LEVEL * 0.7, al_get_bitmap_width(match->ui.staminaBar) * ratio, al_get_bitmap_height(match->ui.staminaBar) * ratio, 0);
    return 0;
}

//winner2 eh diferente de NULL se for empate
void roundEnd(ALLEGRO_DISPLAY* disp,ALLEGRO_FONT* font ,MATCH* match, PLAYER* winner, PLAYER* winner2, ALLEGRO_EVENT_QUEUE* queue, ALLEGRO_EVENT event){
    if(winner && winner2){//Empate
        al_clear_to_color(al_map_rgb(0,0,0));
        al_draw_text(font, al_map_rgb(255, 255, 255), MAX_X / 2, HEADER_LEVEL, ALLEGRO_ALIGN_CENTER, "ROUND OVER");
        al_draw_text(font, al_map_rgb(255, 255, 255), MAX_X / 2, MAX_Y / 2, ALLEGRO_ALIGN_CENTER, "TIE");
    }
    else if(winner->rounds == 1){
        al_clear_to_color(al_map_rgb(0,0,0));
        al_draw_text(font, al_map_rgb(255, 255, 255), MAX_X / 2, HEADER_LEVEL, ALLEGRO_ALIGN_CENTER, "ROUND OVER");
        if(winner == match->P1)
            al_draw_text(font, al_map_rgb(255, 255, 255), MAX_X / 2, MAX_Y / 2, ALLEGRO_ALIGN_CENTER, "PLAYER 1 WINS");
        else
            al_draw_text(font, al_map_rgb(255, 255, 255), MAX_X / 2, MAX_Y / 2, ALLEGRO_ALIGN_CENTER, "PLAYER 2 WINS");
    }
    else{//Match end
        al_clear_to_color(al_map_rgb(0,0,0));
        al_draw_text(font, al_map_rgb(255, 255, 255), MAX_X / 2, HEADER_LEVEL, ALLEGRO_ALIGN_CENTER, "MATCH OVER");
        if(winner == match->P1)
            al_draw_text(font, al_map_rgb(255, 255, 255), MAX_X / 2, MAX_Y / 2, ALLEGRO_ALIGN_CENTER, "PLAYER 1 WINS");
        else
            al_draw_text(font, al_map_rgb(255, 255, 255), MAX_X / 2, MAX_Y / 2, ALLEGRO_ALIGN_CENTER, "PLAYER 2 WINS");
    }
    roundReset(match);
    al_draw_text(font, al_map_rgb(255, 255, 255), MAX_X / 2, FOOTER_LEVEL, ALLEGRO_ALIGN_CENTER, "PRESS SPACE TO CONTINUE");
    al_flip_display();

    pressSpace(queue, &event);
    return;
}

void drawRounds(PLAYER* player1, PLAYER* player2){
    //PLAYER1
    if(player1->rounds <= 1)
        al_draw_circle(MAX_X * 0.35,HEADER_LEVEL * 0.875, HEADER_LEVEL*0.125, al_map_rgb(255,255,255), -1);
    else
        al_draw_filled_circle(MAX_X * 0.35,HEADER_LEVEL * 0.875, HEADER_LEVEL*0.125, al_map_rgb(255,255,255));
    if(player1->rounds == 0)
        al_draw_circle(MAX_X * 0.40,HEADER_LEVEL * 0.875, HEADER_LEVEL * 0.125,al_map_rgb(255,255,255), -1);
    else
        al_draw_filled_circle(MAX_X * 0.40,HEADER_LEVEL * 0.875, HEADER_LEVEL * 0.125,al_map_rgb(255,255,255));
    //PLAYER2
    if(player2->rounds == 0)
        al_draw_circle(MAX_X * 0.60,HEADER_LEVEL * 0.875, HEADER_LEVEL * 0.125,al_map_rgb(255,255,255), -1);
    else
        al_draw_filled_circle(MAX_X * 0.60,HEADER_LEVEL * 0.875, HEADER_LEVEL * 0.125,al_map_rgb(255,255,255));
    if(player2->rounds <= 1)
        al_draw_circle(MAX_X * 0.65,HEADER_LEVEL * 0.875, HEADER_LEVEL * 0.125,al_map_rgb(255,255,255), -1);
    else
        al_draw_filled_circle(MAX_X * 0.65,HEADER_LEVEL * 0.875, HEADER_LEVEL * 0.125,al_map_rgb(255,255,255));
    return;
}

short pauseMenu(ALLEGRO_EVENT_QUEUE* queue,ALLEGRO_EVENT* event, ALLEGRO_FONT* font, MATCH* match){
    bool select[3];//vetor para selecionar
    short i = 0;
    select[0] = true; select[1] = false; select[2] = false;

    while(1){
        al_clear_to_color(al_map_rgb(0,0,0));
        // al_draw_filled_rectangle(0, 0, MAX_X, MAX_Y, al_map_rgba(171,165 ,155, 125));
        if(select[0])
            al_draw_text(font, al_map_rgb(255, 198, 68), MAX_X / 2, MAX_Y * 0.4, ALLEGRO_ALIGN_CENTER, "RESUME");
        else
            al_draw_text(font, al_map_rgb(255, 255, 255), MAX_X / 2, MAX_Y * 0.4, ALLEGRO_ALIGN_CENTER, "RESUME");
        if(select[1])
            al_draw_text(font, al_map_rgb(255, 198, 68), MAX_X / 2, MAX_Y / 2, ALLEGRO_ALIGN_CENTER, "MAIN MENU");
        else
            al_draw_text(font, al_map_rgb(255, 255, 255), MAX_X / 2, MAX_Y / 2, ALLEGRO_ALIGN_CENTER, "MAIN MENU");
        if(select[2])
            al_draw_text(font, al_map_rgb(255, 198, 68), MAX_X / 2, MAX_Y * 0.6, ALLEGRO_ALIGN_CENTER, "EXIT");
        else
            al_draw_text(font, al_map_rgb(255, 255, 255), MAX_X / 2, MAX_Y * 0.6, ALLEGRO_ALIGN_CENTER, "EXIT");
        al_flip_display();
        al_wait_for_event(queue, event);
        //seleciona no menu
        if(event->type == ALLEGRO_EVENT_KEY_DOWN){
            if(event->keyboard.keycode == ALLEGRO_KEY_ESCAPE)
                return resume;//Sai do menu
            else if(event->keyboard.keycode == ALLEGRO_KEY_ENTER)
                return i;//retorna opcao atual
            else if(event->keyboard.keycode == ALLEGRO_KEY_UP){
                if(i != 0){
                    select[i] = false;
                    i--;
                    select[i] = true;
                }
            }
            else if(event->keyboard.keycode == ALLEGRO_KEY_DOWN){
                if(i != 2){
                    select[i] = false;
                    i++;
                    select[i] = true;
                }
            }
        }
    }
    return 0;

}
