#include "street.h"

/*
 * ARQUIVO DE FUNCOES DE DRAW NA TELA DO JOGO
 * POSSUI TAMBÉM MENUS E SEUS CONTROLES
 */

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
    al_draw_scaled_bitmap(shadow,0,0,al_get_bitmap_width(shadow),
                                      al_get_bitmap_height(shadow),
                                      player->x - player->length,
                                      player->y + player->height*0.4,
                                      al_get_bitmap_width(shadow) * ratio,al_get_bitmap_height(shadow) * ratio,
                                      0);
    al_destroy_bitmap(shadow);
    return;
}

void drawPlayer(PLAYER* player, float ratio, short flag){
    if(al_get_timer_started(player->damageState))
        al_draw_tinted_scaled_bitmap(player->fighter.sprite[player->fighter.currentFrame],
                                      al_map_rgb(255, 120, 120),0,0,
                                      al_get_bitmap_width(player->fighter.sprite[player->fighter.currentFrame]),
                                      al_get_bitmap_height(player->fighter.sprite[player->fighter.currentFrame]),
                                      player->x - player->length * 2, player->y - player->height,
                                      al_get_bitmap_width(player->fighter.sprite[player->fighter.currentFrame]) * ratio,
                                      al_get_bitmap_height(player->fighter.sprite[player->fighter.currentFrame]) * ratio,
                                      flag);
    else if(al_get_timer_started(player->blockState))
        al_draw_tinted_scaled_bitmap(player->fighter.sprite[player->fighter.currentFrame],
                                      al_map_rgb(120, 120, 255),0,0,
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
    float ratio = (MAX_X * 0.375) / 82;//82 é o tamanho do bitmap do monk que foi o que usei como base do tamanho
    if(AT_LEFT(player1->x, player2->x)){
        drawPlayer(player1, ratio, 0);
        drawPlayer(player2, ratio, ALLEGRO_FLIP_HORIZONTAL);
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
                                        aux->x , aux->y - aux->side * 1.5,
                                        al_get_bitmap_width(player->spritesProjs[aux->currentFrame]) * ratio,
                                        al_get_bitmap_height(player->spritesProjs[aux->currentFrame]) * ratio,
                                        0);
            else
                al_draw_scaled_bitmap(player->spritesProjs[aux->currentFrame],
                                        0,0,
                                        al_get_bitmap_width(player->spritesProjs[aux->currentFrame]),
                                        al_get_bitmap_height(player->spritesProjs[aux->currentFrame]),
                                        aux->x , aux->y - aux->side * 1.5,
                                        al_get_bitmap_width(player->spritesProjs[aux->currentFrame]) * ratio,
                                        al_get_bitmap_height(player->spritesProjs[aux->currentFrame]) * ratio,
                                        ALLEGRO_FLIP_HORIZONTAL);
            aux = aux->next;
        }
    }
    return;
}

short drawUI(ALLEGRO_DISPLAY* disp, ALLEGRO_FONT* font, ALLEGRO_EVENT_QUEUE* queue, ALLEGRO_EVENT event ,MATCH* match,PLAYER* player1, PLAYER* player2){
    //VIDA P1
    if(player1->health > 0)
        al_draw_filled_rounded_rectangle((MAX_X * 0.45)*(1.0275 - (float)player1->health/BASE_HEALTH),0, MAX_X*0.454, HEADER_LEVEL / 2, 10, 15,al_map_rgb(255, 0, 0)); //VIDA P1
    else{
        player2->rounds++;
        roundEnd(disp, font, match, player2, NULL, queue, event);
        if(player2->rounds == 2)
            return mainMenu;
    }
    //VIDA P2
    if(player2->health > 0)
        al_draw_filled_rounded_rectangle(MAX_X*0.5475, 0 ,(MAX_X * 0.989) - ((1 - (float)player2->health/BASE_HEALTH) * MAX_X * 0.425), HEADER_LEVEL / 2, 10, 15,al_map_rgb(255, 0, 0)); 
    else{
        player1->rounds++;
        roundEnd(disp, font, match, player1, NULL, queue, event);
        if(player1->rounds == 2)
            return mainMenu;
    }

    if(player1->stamina > 0)
        al_draw_filled_rectangle((MAX_X * 0.30)*(1.05 - (float)player1->stamina/BASE_STAMINA),
                            HEADER_LEVEL * 0.75, MAX_X*0.31, HEADER_LEVEL,al_map_rgb(0, 255, 0)); //Stamina P1
    if(player2->stamina > 0)
        al_draw_filled_rectangle(MAX_X*0.6925, HEADER_LEVEL * 0.75,
                            (MAX_X * 0.99) - ((1 - (float)player2->stamina/BASE_STAMINA) * MAX_X * 0.30),
                            HEADER_LEVEL,al_map_rgb(0, 255, 0)); //Stamina P2

    //BARRAS DE VIDA
    float ratio = (MAX_X*0.48) / al_get_bitmap_width(match->ui.bar);
    al_draw_scaled_bitmap(match->ui.bar, 0, 0, al_get_bitmap_width(match->ui.bar), al_get_bitmap_height(match->ui.bar),
                            -10, 0, al_get_bitmap_width(match->ui.bar) * ratio, al_get_bitmap_height(match->ui.bar) * ratio, 0);
    al_draw_scaled_bitmap(match->ui.bar, 0, 0, al_get_bitmap_width(match->ui.bar), al_get_bitmap_height(match->ui.bar),
                            MAX_X * 0.525, 0, al_get_bitmap_width(match->ui.bar) * ratio, al_get_bitmap_height(match->ui.bar) * ratio, 0);
    //BARRAS DE STAMINA
    ratio = (MAX_X*0.32) / al_get_bitmap_width(match->ui.bar);
    al_draw_scaled_bitmap(match->ui.bar, 0, 0, al_get_bitmap_width(match->ui.bar), al_get_bitmap_height(match->ui.bar),
                            0, HEADER_LEVEL * 0.7, al_get_bitmap_width(match->ui.bar) * ratio, al_get_bitmap_height(match->ui.bar) * ratio, 0);
    al_draw_scaled_bitmap(match->ui.bar, 0, 0, al_get_bitmap_width(match->ui.bar), al_get_bitmap_height(match->ui.bar),
                            MAX_X*0.68, HEADER_LEVEL * 0.7, al_get_bitmap_width(match->ui.bar) * ratio, al_get_bitmap_height(match->ui.bar) * ratio, 0);
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
    al_draw_text(font, al_map_rgb(255, 198, 68), MAX_X / 2, FOOTER_LEVEL, ALLEGRO_ALIGN_CENTER, "PRESS SPACE TO CONTINUE");
    al_flip_display();

    pressSpace(queue, &event);
    return;
}

void drawRounds(PLAYER* player1, PLAYER* player2){
    //PLAYER1
    if(player1->rounds <= 1)
        al_draw_circle(MAX_X * 0.35,HEADER_LEVEL * 0.875, HEADER_LEVEL*0.125, al_map_rgb(255,255,255), MAX_X * 0.0025);
    else
        al_draw_filled_circle(MAX_X * 0.35,HEADER_LEVEL * 0.875, HEADER_LEVEL*0.125, al_map_rgb(255,255,255));
    if(player1->rounds == 0)
        al_draw_circle(MAX_X * 0.40,HEADER_LEVEL * 0.875, HEADER_LEVEL * 0.125,al_map_rgb(255,255,255), MAX_X * 0.0025);
    else
        al_draw_filled_circle(MAX_X * 0.40,HEADER_LEVEL * 0.875, HEADER_LEVEL * 0.125,al_map_rgb(255,255,255));
    //PLAYER2
    if(player2->rounds == 0)
        al_draw_circle(MAX_X * 0.60,HEADER_LEVEL * 0.875, HEADER_LEVEL * 0.125,al_map_rgb(255,255,255), MAX_X * 0.0025);
    else
        al_draw_filled_circle(MAX_X * 0.60,HEADER_LEVEL * 0.875, HEADER_LEVEL * 0.125,al_map_rgb(255,255,255));
    if(player2->rounds <= 1)
        al_draw_circle(MAX_X * 0.65,HEADER_LEVEL * 0.875, HEADER_LEVEL * 0.125,al_map_rgb(255,255,255), MAX_X * 0.0025);
    else
        al_draw_filled_circle(MAX_X * 0.65,HEADER_LEVEL * 0.875, HEADER_LEVEL * 0.125,al_map_rgb(255,255,255));
    return;
}

//menu de pause 
short pauseMenu(ALLEGRO_EVENT_QUEUE* queue,ALLEGRO_EVENT* event, ALLEGRO_FONT* font, MATCH* match){
    bool select[3];//vetor para selecionar
    short i = 0;
    select[0] = true; select[1] = false; select[2] = false;

    while(1){
        al_clear_to_color(al_map_rgb(0,0,0));
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

//menu de selecao de personagem
short selectFighter(ALLEGRO_EVENT_QUEUE* queue , ALLEGRO_EVENT* event, ALLEGRO_FONT* font, char* fighter){
    bool select[2];
    select[0] = 1;select[1] = 0;
    short i = 0;
    while(1){
        al_clear_to_color(al_map_rgb(0,0,0));
        al_draw_text(font, al_map_rgb(255, 198, 68), MAX_X / 2, HEADER_LEVEL, ALLEGRO_ALIGN_CENTER, fighter);
        if(select[0])
            al_draw_text(font, al_map_rgb(255, 198, 68), MAX_X / 2, MAX_Y * 0.45, ALLEGRO_ALIGN_CENTER, "MONK");
        else
            al_draw_text(font, al_map_rgb(255, 255, 255), MAX_X / 2, MAX_Y * 0.45, ALLEGRO_ALIGN_CENTER, "MONK");
        if(select[1])
            al_draw_text(font, al_map_rgb(255, 198, 68), MAX_X / 2, MAX_Y * 0.55, ALLEGRO_ALIGN_CENTER, "BRAWLER");
        else
            al_draw_text(font, al_map_rgb(255, 255, 255), MAX_X / 2, MAX_Y * 0.55, ALLEGRO_ALIGN_CENTER, "BRAWLER");
        al_flip_display();
        al_wait_for_event(queue, event);
        if(event->type == ALLEGRO_EVENT_KEY_DOWN){
            if(event->keyboard.keycode == ALLEGRO_KEY_ENTER){
                return i;
            }
            else if(event->keyboard.keycode == ALLEGRO_KEY_UP){   
                if(i != 0){
                    select[i] = false;
                    i--;
                    select[i] = true;
                }
            }
            else if(event->keyboard.keycode == ALLEGRO_KEY_DOWN){
                if(i != 1){
                    select[i] = false;
                    i++;
                    select[i] = true;
                }
            }
        }
    }

}

//menu de selecao de mapa
short selectMap(ALLEGRO_EVENT_QUEUE* queue, ALLEGRO_EVENT* event, ALLEGRO_FONT* font){
    bool select[3];
    select[0] = 1;select[1] = 0;select[2] = 0;
    short i = 0;
    while(1){
        al_clear_to_color(al_map_rgb(0,0,0));
        al_draw_text(font, al_map_rgb(255, 198, 68), MAX_X / 2, HEADER_LEVEL, ALLEGRO_ALIGN_CENTER, "MAP");
        if(select[0])
            al_draw_text(font, al_map_rgb(255, 198, 68), MAX_X / 2, MAX_Y * 0.4, ALLEGRO_ALIGN_CENTER, "FOREST");
        else
            al_draw_text(font, al_map_rgb(255, 255, 255), MAX_X / 2, MAX_Y * 0.4, ALLEGRO_ALIGN_CENTER, "FOREST");
        if(select[1])
            al_draw_text(font, al_map_rgb(255, 198, 68), MAX_X / 2, MAX_Y / 2, ALLEGRO_ALIGN_CENTER, "DOJO");
        else
            al_draw_text(font, al_map_rgb(255, 255, 255), MAX_X / 2, MAX_Y / 2, ALLEGRO_ALIGN_CENTER, "DOJO");
        if(select[2])
            al_draw_text(font, al_map_rgb(255, 198, 68), MAX_X / 2, MAX_Y * 0.6, ALLEGRO_ALIGN_CENTER, "TRAIL");
        else
            al_draw_text(font, al_map_rgb(255, 255, 255), MAX_X / 2, MAX_Y * 0.6, ALLEGRO_ALIGN_CENTER, "TRAIL");
        al_flip_display();
        al_wait_for_event(queue, event);
        if(event->type == ALLEGRO_EVENT_KEY_DOWN){
            if(event->keyboard.keycode == ALLEGRO_KEY_ENTER){
                  return i;
            }
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

}
//menu principal do jogo
short drawMainMenu(ALLEGRO_EVENT_QUEUE* queue, ALLEGRO_EVENT* event, ALLEGRO_FONT* font){
    bool select[3];
    select[0] = 1;select[1] = 0;select[2] = 0;
    short i = 0;
    while(1){
        al_clear_to_color(al_map_rgb(0,0,0));
        if(select[0])
            al_draw_text(font, al_map_rgb(255, 198, 68), MAX_X / 2, MAX_Y * 0.4, ALLEGRO_ALIGN_CENTER, "SINGLE PLAYER");
        else
            al_draw_text(font, al_map_rgb(255, 255, 255), MAX_X / 2, MAX_Y * 0.4, ALLEGRO_ALIGN_CENTER, "SINGLE PLAYER");
        if(select[1])
            al_draw_text(font, al_map_rgb(255, 198, 68), MAX_X / 2, MAX_Y / 2, ALLEGRO_ALIGN_CENTER, "MULTIPLAYER");
        else
            al_draw_text(font, al_map_rgb(255, 255, 255), MAX_X / 2, MAX_Y / 2, ALLEGRO_ALIGN_CENTER, "MULTIPLAYER");
        if(select[2])
            al_draw_text(font, al_map_rgb(255, 198, 68), MAX_X / 2, MAX_Y * 0.6, ALLEGRO_ALIGN_CENTER, "EXIT");
        else
            al_draw_text(font, al_map_rgb(255, 255, 255), MAX_X / 2, MAX_Y * 0.6, ALLEGRO_ALIGN_CENTER, "EXIT");
        al_flip_display();
        al_wait_for_event(queue, event);
        if(event->type == ALLEGRO_EVENT_KEY_DOWN){
            if(event->keyboard.keycode == ALLEGRO_KEY_ESCAPE)
                return quit;//Sai do menu
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
}

//proximo frame da animacao de personagem
void nextFrame(PLAYER* player, ALLEGRO_EVENT event){
    if(event.timer.source == player->fighter.frameMovement)
        player->fighter.currentFrame = (player->fighter.currentFrame + 1) % player->fighter.size;//proximo frame
    else if(event.timer.source == player->fighter.frameAttack && player->fighter.currentFrame < player->fighter.size - 1)
        player->fighter.currentFrame++;
    return;
}

//proximo frame de animacao de projetil
void nextFrameProj(PLAYER* player){
    if(player->projs){
        PROJECTILE* aux = player->projs;
        while(aux){
            aux->currentFrame = (aux->currentFrame + 1) % player->sizeSprites;
            aux = aux->next;
        }
    }
    return ;
}
