#include "street.h"
#include <allegro5/events.h>
#include <allegro5/timer.h>
#include <stdbool.h>
#include <time.h>

/*
KEYBINDS* createKeys(){
    KEYBINDS* key = NULL;

    key = (KEYBINDS*)malloc(sizeof(KEYBINDS));
    if(!key)
        return NULL;
    key->kick = false;
    key->punch = false;
    key->special = false;

    return key;
}
*/

void pressSpace(ALLEGRO_EVENT_QUEUE* queue, ALLEGRO_EVENT* event){
    while(1){
        al_wait_for_event(queue, event);
        if(event->type == ALLEGRO_EVENT_KEY_UP && event->keyboard.keycode == ALLEGRO_KEY_SPACE)
            break;
    }
    return;
}

//winner2 eh diferente de NULL se for empate
void roundEnd(ALLEGRO_DISPLAY* disp,ALLEGRO_FONT* font ,MATCH* match, PLAYER* winner, PLAYER* winner2){
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
    match->P1->health = BASE_HEALTH;
    match->P1->stamina = BASE_STAMINA;
    match->P2->health = BASE_HEALTH;
    match->P2->stamina = BASE_STAMINA;
    al_stop_timer(match->P1->cooldownAttack);
    al_stop_timer(match->P1->cooldownProj);
    al_stop_timer(match->P2->cooldownAttack);
    al_stop_timer(match->P2->cooldownProj);
    match->time = MATCH_LENGTH;
    al_draw_text(font, al_map_rgb(255, 255, 255), MAX_X / 2, FOOTER_LEVEL, ALLEGRO_ALIGN_CENTER, "PRESS SPACE TO CONTINUE");
    al_flip_display();
    return;
}

bool inRangeX(float x, PLAYER* player2){
    if(x <= player2->x + (player2->length / 2) && x >= player2->x - (player2->length / 2))
        return true;
    return false;
}

bool inRangeY(float y, PLAYER* player2){
    if(y <= player2->y + (player2->height / 2) && y >= player2->y - (player2->height / 2))
        return true;
    return false;
}

void moveUp(PLAYER* player){

    player->stick->up = !player->stick->up;

    //Se estiver agachado nao faz nada(Prioridade)
    if(player->stick->up){
        if(player->state == stand){
            player->state = jump;
            player->directionY = up;
        }
        else if(player->state == walkB){
            player->state = jumpB;
            player->directionY = up;
        }
        else if(player->state == walkF){
            player->state = jumpF;
            player->directionY = up;
        }
    }
    return;
}

void moveDown(PLAYER* player, PLAYER* player2){

    player->stick->down = !player->stick->down;

    //Se estiver pulando nao faz nada
    if(player->stick->down){
        if(player->state == stand || player->state == walkB || player->state == walkF){
            player->state = crouch;
            player->directionX = none;
        }
    }
    else if(!player->stick->down && player->state == crouch){
        if(player->stick->left){
            if(AT_LEFT(player, player2))
                player->state = walkB;
            else
                player->state = walkF;
            player->directionX = left;
            return;
        }
        if(player->stick->right){
            if(AT_RIGHT(player, player2))
                player->state = walkB;
            else
                player->state = walkF;
            player->directionX = right;
            return;
        }
        player->state = stand;

    }

    return;
}

void moveLeft(PLAYER* player1, PLAYER* player2){

    player1->stick->left = !player1->stick->left;

    //Dependendo da posicao do outro player recebe estado
    if(player1->stick->left && player1->state == stand){
        if(AT_LEFT(player1->x, player2->x))
            player1->state = walkB;
        else
            player1->state = walkF;
        player1->directionX = left;
    }
    //se pressionar enquanto anda na outra direcao
    else if(player1->stick->left && player1->directionX == right && (player1->state == walkB || player1->state == walkF)){
        player1->state = stand;
        player1->directionX = none;
    }
    //Se soltar e estar andando para esquerda para de andar
    else if(!player1->stick->left && (player1->state == walkB || player1->state == walkF) && !player1->stick->right){
        player1->state = stand;
        player1->directionX = none;
    }
    //se tiver as duas direcoes apertadas e soltar uma
    else if(player1->stick->right && player1->directionY == none){
        player1->directionX = right;
        if(AT_RIGHT(player1, player2))
            player1->state = walkB;
        else
            player1->state = walkF;
    }

    return;
}

void moveRight(PLAYER* player1, PLAYER* player2){

    player1->stick->right = !player1->stick->right;

    //Dependendo da posicao do outro player recebe estado
    if(player1->stick->right && player1->state == stand){
        if(AT_RIGHT(player1->x, player2->x))
            player1->state = walkB;
        else
            player1->state = walkF;
        player1->directionX = right;
    }
    //Se pressionar enquanto anda na outra direcao
    else if(player1->stick->right && player1->directionX == left && (player1->state == walkB || player1->state == walkF)){
        player1->state = stand;
        player1->directionX = none;
    }
     //Se soltar e estar andando para esquerda para de andar
    else if(!player1->stick->right && (player1->state == walkB || player1->state == walkF) && !player1->stick->left){
        player1->state = stand;
        player1->directionX = none;
    }
        //se tiver as duas direcoes apertadas e soltar uma
    else if(player1->stick->left && player1->directionY == none){
        player1->directionX = left;
        if(AT_LEFT(player1, player2))
            player1->state = walkB;
        else
            player1->state = walkF;
    }

    return;
}

//Calcula se movimento é valido no eixo Y
bool isMovementValidUp(PLAYER* player1, PLAYER* player2){
    //Se tiver movimento para cima
    if(player1->y - (player1->height / 2) - player1->speedY < 0)
        return false;
    //Se estiver em baixo do outro player
    if(player1->y > player2->y && inRangeX(player1->x, player2))
        if(player1->y - (player1->height / 2) - player1->speedY < (player2->y + player2->height / 2))
            return false;
    return true;
}

//Calcula se movimento é valido no eixo Y
bool isMovementValidDown(PLAYER* player1, PLAYER* player2){

    if((player1->y + (player1->height / 2) + player1->speedY) > GROUND_LEVEL)
        return false;
    //Se tiver encima do outro player
    if(player1->y < player2->y && inRangeX(player1->x, player2))
        if(player1->y + (player1->height / 2) + player1->speedY > (player2->y - player2->height / 2))
            return false;
    return true;
}

//Testa se movemento nao quebra nenhum limite
bool isMovementValidLeft(PLAYER* player1, PLAYER* player2){

    //Se tiver movimento para cima
    /*if(player1->directionY == up ){
        if(!isMovementValidUp(player1, player2))
            return false;
    }
    //Se tiver movimento para baixo
    else if(player1->directionY == down)
        if(!isMovementValidDown(player1, player2))
            return false;
    */
    if(player1->x - (player1->length / 2) - player1->speedX < 0)
        return false;
    //Se estiver na direita e no intervalo da altura do outro player
    if(player1->x > player2->x && inRangeY(player1->y, player2)){
        if(player1->x - (player1->length / 2) - player1->speedX < player2->x + player2->length / 2)
            return false;
    }
    return true;
}

bool isMovementValidRight(PLAYER* player1, PLAYER* player2){
    /*
    //Se tiver movimento para cima
    if(player1->directionY == up){
        if(!isMovementValidUp(player1, player2))
            return false;
    }
    //Se tiver movimento para baixo
    else if(player1->directionY == down)
        if(!isMovementValidDown(player1,player2))
            return false;
    */
    if(player1->x + (player1->length / 2) + player1->speedX > MAX_X)
        return false;
    //Se outro player estar na esquerda e no intervalo da altura do outro player
    if(player1->x < player2->x && inRangeY(player1->y, player2)){
        if(player1->x + (player1->length / 2) + player1->speedX > player2->x - player2->length / 2)
            return false;
    }
    return true;
}

//Aplica dano dependendo de oque fex o hit
void hitApply(PROJECTILE* projectile, ATTACK* attack,PLAYER* killer, PLAYER* victim){
    if(projectile){
        victim->health -= projectile->dmg;
        destroyProjectile(&killer->projs, projectile);
    }
    else if(attack){}
    return;
}

//Move projetil
void moveProjectile(PLAYER* player1, PLAYER* player2){
    PROJECTILE* aux = player1->projs;
    if(aux){
        while(aux){
            if(aux->x < 0 || aux->x > MAX_X)//Miss
                destroyProjectile(&player1->projs, aux);
            else if(player2->projs){//Checa se acerta outros projeteis
                PROJECTILE* auxP2 = player2->projs;
                while (auxP2) {
                    if(aux->x <= auxP2->x + auxP2->side / 2 && aux->x >= auxP2->x - auxP2->side / 2){//Hit projetil
                        destroyProjectile(&player1->projs, aux);
                        destroyProjectile(&player2->projs, auxP2);
                    }
                    auxP2 = auxP2->next;
                }
            }
            else if(inRangeX(aux->x, player2) && inRangeY(aux->y, player2) && player2->state != crouch) //Hit
                hitApply(aux, NULL,player1, player2);

            else{
                if(aux->direction == left)
                    aux->x -= aux->speed;
                else
                    aux->x += aux->speed;
            }
            aux = aux->next;
        }
    }
    return;
}

void movePlayer(MATCH* match, PLAYER* player1, PLAYER* player2){
    if(player1->directionX == right){
        if(isMovementValidRight(player1, player2)){
            player1->x += player1->speedX;
        }
    }
    else if(player1->directionX == left){
        if(isMovementValidLeft(player1, player2))
            player1->x -= player1->speedX;
    }
    if(player1->directionY == up){
        if(isMovementValidUp(player1, player2)){
            player1->y -= player1->speedY;
            player1->speedY -= (BASE_GRAV);
            if(player1->speedY < 0)
                player1->directionY = down;
        }
    }
    else if(player1->directionY == down){
        if(isMovementValidDown(player1, player2)){
            player1->y -= player1->speedY;
            player1->speedY -= BASE_GRAV;
        }
        else if (player1->y > player2->y - player2->height / 2){ //Chegou no chão
            player1->y = GROUND_LEVEL - player1->height / 2;
            player1->speedY = BASE_SPEEDY;// * match->speedMult;
            player1->directionY = none;
            if(player1->directionX && !player1->stick->right && !player1->stick->left)
                player1->directionX = none;
            else if(player1->stick->right){
                if(AT_RIGHT(player1, player2))
                    player1->state = walkB;
                else
                    player1->state = walkF;
                return;
            }
            else if(player1->stick->left){
                if(AT_LEFT(player1, player2))
                    player1->state = walkB;
                else
                    player1->state = walkF;
                return;
            }
            player1->state = stand;
        }
    }
    return;
}

void checkHitAttack(PLAYER* attacker, PLAYER* victim){
    if(!attacker->attacks){
        return;
    }
    if(attacker->attacks->id == punch && inRangeX(attacker->attacks->x, victim) && inRangeY(attacker->attacks->y, victim) && victim->state != walkB && victim->state != crouch)//hit
        victim->health -= attacker->attacks->dmg;
    else if(attacker->attacks->id == kick && inRangeX(attacker->attacks->x, victim) && inRangeY(attacker->attacks->y, victim) && victim->state != crouch)
        victim->health -= attacker->attacks->dmg;
    return; 
}
