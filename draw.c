#include "street.h"
#include <allegro5/allegro_primitives.h>
#include <allegro5/bitmap.h>
#include <allegro5/bitmap_draw.h>
#include <allegro5/bitmap_io.h>
#include <allegro5/color.h>
#include <allegro5/keycodes.h>
#include <stdbool.h>

void drawShadow(PLAYER* player){
    if(player->directionY != none)
        return;
    ALLEGRO_BITMAP* shadow = al_load_bitmap("images/fighters/shadow.png");
    al_draw_scaled_bitmap(shadow,0,0,al_get_bitmap_width(shadow),al_get_bitmap_height(shadow),player->x - player->length,player->y + player->height*0.35,al_get_bitmap_width(shadow) *4.75,al_get_bitmap_height(shadow)*4.75,0);
    al_destroy_bitmap(shadow);
    return;
}

void drawCharacter(PLAYER* player1, PLAYER* player2){
    if(AT_LEFT(player1->x, player2->x)){
        al_draw_scaled_bitmap(player1->fighter.sprite[player1->fighter.currentFrame],
                                        0,0,
                                        al_get_bitmap_width(player1->fighter.sprite[player1->fighter.currentFrame]),
                                        al_get_bitmap_height(player1->fighter.sprite[player1->fighter.currentFrame]),
                                        player1->x - player1->length * 2, player1->y - player1->height,
                                        al_get_bitmap_width(player1->fighter.sprite[player1->fighter.currentFrame]) * 2.5,
                                        al_get_bitmap_height(player1->fighter.sprite[player1->fighter.currentFrame]) * 2.5,
                                        0);
        al_draw_scaled_bitmap(player2->fighter.sprite[player2->fighter.currentFrame],
                                        0,0,
                                        al_get_bitmap_width(player2->fighter.sprite[player2->fighter.currentFrame]),
                                        al_get_bitmap_height(player2->fighter.sprite[player2->fighter.currentFrame]),
                                        player2->x - player2->length * 2, player2->y - player2->height,
                                        al_get_bitmap_width(player2->fighter.sprite[player2->fighter.currentFrame]) * 2.5,
                                        al_get_bitmap_height(player2->fighter.sprite[player2->fighter.currentFrame]) * 2.5,
                                        ALLEGRO_FLIP_HORIZONTAL);
                // al_draw_bitmap(player2->fighter.sprite[player2->fighter.currentFrame],player2->x - player2->length / 2, player2->y - player2->height / 2 , ALLEGRO_FLIP_HORIZONTAL);
            }
    else{
        al_draw_scaled_bitmap(player1->fighter.sprite[player1->fighter.currentFrame],
                                        0,0,
                                        al_get_bitmap_width(player1->fighter.sprite[player1->fighter.currentFrame]),
                                        al_get_bitmap_height(player1->fighter.sprite[player1->fighter.currentFrame]),
                                        player1->x - player1->length * 2, player1->y - player1->height,
                                        al_get_bitmap_width(player1->fighter.sprite[player1->fighter.currentFrame]) * 2.5,
                                        al_get_bitmap_height(player1->fighter.sprite[player1->fighter.currentFrame]) * 2.5,
                                        ALLEGRO_FLIP_HORIZONTAL);
                // al_draw_bitmap(player1->fighter.sprite[player1->fighter.currentFrame],player1->x - player1->length / 2, player1->y - player1->height / 2 , ALLEGRO_FLIP_HORIZONTAL);
                // al_draw_bitmap(player2->fighter.sprite[player2->fighter.currentFrame],player2->x - player2->length / 2, player2->y - player2->height / 2 , 0);
        al_draw_scaled_bitmap(player2->fighter.sprite[player2->fighter.currentFrame],
                                        0,0,
                                        al_get_bitmap_width(player2->fighter.sprite[player2->fighter.currentFrame]),
                                        al_get_bitmap_height(player2->fighter.sprite[player2->fighter.currentFrame]),
                                        player2->x - player2->length *2 , player2->y - player2->height,
                                        al_get_bitmap_width(player2->fighter.sprite[player2->fighter.currentFrame]) * 2.5,
                                        al_get_bitmap_height(player2->fighter.sprite[player2->fighter.currentFrame]) * 2.5,
                                        0);
            }
    return;
}

void drawProjectile(PLAYER* player){
    if(player->projs){
        PROJECTILE* aux = player->projs;
        while(aux){
            if(aux->direction == right)
            //al_draw_filled_rectangle(aux->x - aux->side / 2, aux->y - aux->side, aux->x + aux->side,player1->projs->y + aux->side, al_map_rgb(0,255,0));
                al_draw_scaled_bitmap(player->spritesProjs[aux->currentFrame],
                                        0,0,
                                        al_get_bitmap_width(player->spritesProjs[aux->currentFrame]),
                                        al_get_bitmap_height(player->spritesProjs[aux->currentFrame]),
                                        aux->x , aux->y - aux->side,
                                        al_get_bitmap_width(player->spritesProjs[aux->currentFrame]) * 3.5,
                                        al_get_bitmap_height(player->spritesProjs[aux->currentFrame]) * 3.5,
                                        0);
            else
                al_draw_scaled_bitmap(player->spritesProjs[aux->currentFrame],
                                        0,0,
                                        al_get_bitmap_width(player->spritesProjs[aux->currentFrame]),
                                        al_get_bitmap_height(player->spritesProjs[aux->currentFrame]),
                                        aux->x , aux->y - aux->side,
                                        al_get_bitmap_width(player->spritesProjs[aux->currentFrame]) * 3.5,
                                        al_get_bitmap_height(player->spritesProjs[aux->currentFrame]) * 3.5,
                                        ALLEGRO_FLIP_HORIZONTAL);
            aux = aux->next;
        }
    }
    return;
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
