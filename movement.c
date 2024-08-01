#include "street.h"

void moveUp(ALLEGRO_EVENT* event, PLAYER* player){
    if(!player->stick->valUp && event && event->type == ALLEGRO_EVENT_KEY_UP)//evita bug se comecar com tecla segurada
        return;
    else
        player->stick->valUp = true;
    player->stick->up = !player->stick->up;
    //Se estiver agachado nao faz nada
    if(player->stick->up){
        if(player->state == stand){
            player->state = jump;
            player->directionY = up;
            player->fighter.newFlag = true;
        }
        else if(player->state == walkB){
            player->state = jumpB;
            player->directionY = up;
            player->fighter.newFlag = true;
        }
        else if(player->state == walkF){
            player->state = jumpF;
            player->directionY = up;
            player->fighter.newFlag = true;
        }
    }
    return;
}

void moveDown(ALLEGRO_EVENT* event, PLAYER* player, PLAYER* player2){
    if(!player->stick->valDown && event && event->type == ALLEGRO_EVENT_KEY_UP)//evita bug se comecar com tecla segurada
        return;
    else
        player->stick->valDown = true;
    player->stick->down = !player->stick->down;
    //Se estiver pulando nao faz nada
    if(player->stick->down){
        if(player->state == stand || player->state == walkB || player->state == walkF){
            player->state = crouch;
            player->directionX = none;
            player->fighter.newFlag = true;
        }
    }
    //se levantar e tiver outra tecla pressionada
    else if(!player->stick->down && player->state == crouch){
        if(player->stick->left){
            if(AT_LEFT(player, player2))
                player->state = walkB;
            else
                player->state = walkF;
            player->directionX = left;
            player->fighter.newFlag = true;
            return;
        }
        if(player->stick->right){
            if(AT_RIGHT(player, player2))
                player->state = walkB;
            else
                player->state = walkF;
            player->directionX = right;
            player->fighter.newFlag = true;
            return;
        }
        player->state = stand;
        player->fighter.newFlag = true;
    }

    return;
}

void moveLeft(ALLEGRO_EVENT* event, PLAYER* player1, PLAYER* player2){
    if(!player1->stick->valLeft && event && event->type == ALLEGRO_EVENT_KEY_UP)//evita bug se comecar com tecla segurada
        return;
    else
        player1->stick->valLeft = true;
    player1->stick->left = !player1->stick->left;
    //Dependendo da posicao do outro player recebe estado
    if(player1->stick->left && player1->state == stand){
        if(AT_LEFT(player1->x, player2->x))
            player1->state = walkB;
        else
            player1->state = walkF;
        player1->directionX = left;
        player1->fighter.newFlag = true;
    }
    //se pressionar enquanto anda na outra direcao
    else if(player1->stick->left && player1->directionX == right && (player1->state == walkB || player1->state == walkF)){
        player1->state = stand;
        player1->directionX = none;
        player1->fighter.newFlag = true;
    }
    //Se soltar e estar andando para esquerda para de andar
    else if(!player1->stick->left && (player1->state == walkB || player1->state == walkF) && !player1->stick->right){
        player1->state = stand;
        player1->directionX = none;
        player1->fighter.newFlag = true;
    }
    //se tiver as duas direcoes apertadas e soltar uma
    else if(player1->stick->right && player1->directionY == none){
        player1->directionX = right;
        if(AT_RIGHT(player1, player2))
            player1->state = walkB;
        else
            player1->state = walkF;
        player1->fighter.newFlag = true;
    }

    return;
}

void moveRight(ALLEGRO_EVENT* event, PLAYER* player1, PLAYER* player2){
    if(!player1->stick->valRight && event && event->type == ALLEGRO_EVENT_KEY_UP)//evita bug se comecar com tecla segurada
        return;
    else
        player1->stick->valRight = true;
    player1->stick->right = !player1->stick->right;
    //Dependendo da posicao do outro player recebe estado
    if(player1->stick->right && player1->state == stand){
        if(AT_RIGHT(player1->x, player2->x))
            player1->state = walkB;
        else
            player1->state = walkF;
        player1->directionX = right;
        player1->fighter.newFlag = true;
    }
    //Se pressionar enquanto anda na outra direcao
    else if(player1->stick->right && player1->directionX == left && (player1->state == walkB || player1->state == walkF)){
        player1->state = stand;
        player1->directionX = none;
        player1->fighter.newFlag = true;
    }
     //Se soltar e estar andando para esquerda para de andar
    else if(!player1->stick->right && (player1->state == walkB || player1->state == walkF) && !player1->stick->left){
        player1->state = stand;
        player1->directionX = none;
        player1->fighter.newFlag = true;
    }
        //Se tiver as duas direcoes apertadas e soltar uma
    else if(player1->stick->left && player1->directionY == none){
        player1->directionX = left;
        if(AT_LEFT(player1, player2))
            player1->state = walkB;
        else
            player1->state = walkF;
        player1->fighter.newFlag = true;
    }

    return;
}

/*movimento do computador se estiver em single player
 * Anda e ataque quando no range e/ou fora de cooldown
 * Se estiver em animação não faz nada
 */
void singlePlayerMovement(PLAYER* player1, PLAYER* player2){
  
    if(al_get_timer_started(player2->attackDuration) || al_get_timer_started(player2->projDuration) || al_get_timer_started(player2->damageState))
        return ;
    if(AT_LEFT(player2->x, player1->x)){
        if(player2->directionX == left)//troca lado
            moveLeft(NULL, player2,player1);
        if(inRangeX(player2->x + player2->length * 1.25, player1)){
            if(player2->directionX == right)
                moveRight(NULL, player2, player1);
            if(!al_get_timer_started(player2->cooldownAttack))//soco
                attackWrapper(player2, player1, punch);
            else if(!al_get_timer_started(player2->cooldownAttackLow) && !al_get_timer_started(player2->attackDuration))//chute
                attackWrapper(player2, player1, kick);
        }
        else if(!al_get_timer_started(player2->cooldownProj)){//projetil
            if(player2->directionX == right)
                moveRight(NULL, player2, player1);
            projectileWrapper(player2, player1);
        }
        else if(player2->directionX != right){//movemento
            moveRight(NULL, player2, player1);
        }
     }
    else{
        if(player2->directionX == right)//troca lado
            moveRight(NULL, player2,player1);
        if(inRangeX(player2->x - player2->length * 1.25, player1)){
            if(player2->directionX == left)
                moveLeft(NULL, player2, player1);
            if(!al_get_timer_started(player2->cooldownAttack))//soco
                attackWrapper(player2, player1, punch);
            else if(!al_get_timer_started(player2->cooldownAttackLow) && !al_get_timer_started(player2->attackDuration))//chute
                attackWrapper(player2, player1, kick);
        }
        else if(!al_get_timer_started(player2->cooldownProj)){//projetil
            if(player2->directionX == left)
                  moveLeft(NULL, player2, player1);
            projectileWrapper(player2, player1);
        }
        else if(player2->directionX != left){//movemento
            moveLeft(NULL, player2, player1);
        }
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

    //esse player1->height sem o /2 eh para manter consistente o check sobre o chao
    if((player1->y + (player1->height * 0.75) + player1->speedY) > GROUND_LEVEL)
        return false;
    //Se tiver encima do outro player
    if(player1->y < player2->y && inRangeX(player1->x, player2))
        if(player1->y + (player1->height / 2) + player1->speedY > (player2->y - player2->height / 2))
            return false;
    return true;
}

//Testa se movemento nao quebra nenhum limite
bool isMovementValidLeft(PLAYER* player1, PLAYER* player2){
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
    if(player1->x + (player1->length / 2) + player1->speedX > MAX_X)
        return false;
    //Se outro player estar na esquerda e no intervalo da altura do outro player
    if(player1->x < player2->x && inRangeY(player1->y, player2)){
        if(player1->x + (player1->length / 2) + player1->speedX > player2->x - player2->length / 2)
            return false;
    }
    return true;
}

//move personagem para direcoes que tiver em suas velocidades x e y
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
            player1->speedY = BASE_SPEEDY;
            player1->directionY = none;
            if(player1->directionX && !player1->stick->right && !player1->stick->left)
                player1->directionX = none;
            else if(player1->stick->right){
                if(AT_RIGHT(player1, player2))
                    player1->state = walkB;
                else
                    player1->state = walkF;
                player1->fighter.newFlag = true;
                return;
            }
            else if(player1->stick->left){
                if(AT_LEFT(player1, player2))
                    player1->state = walkB;
                else
                    player1->state = walkF;
                player1->fighter.newFlag = true;
                return;
            }
            player1->state = stand;
            player1->fighter.newFlag = true;
        }
    }
    return;
}

//Move projetil
void moveProjectile(PLAYER* player1, PLAYER* player2){
    PROJECTILE* aux = player1->projs;
    PROJECTILE* next;
    PROJECTILE* nextP2;
    if(aux){
        while(aux){
            next = aux->next;
            if(aux && (aux->x < 0 || aux->x > MAX_X)){//Miss
                destroyProjectile(&player1->projs, aux);
                aux = NULL;
            }

            else if(player2->projs){//Checa se acerta outros projeteis
                PROJECTILE* auxP2 = player2->projs;
                while (auxP2) {
                    nextP2 = auxP2->next;
                    if(aux->x <= auxP2->x + auxP2->side / 2 && aux->x >= auxP2->x - auxP2->side / 2){//Hit
                        destroyProjectile(&player1->projs, aux);
                        destroyProjectile(&player2->projs, auxP2);
                        aux = NULL;
                    }
                    auxP2 = nextP2;
                }
            }

            if(aux && inRangeX(aux->x, player2) && inRangeY(aux->y, player2) && player2->state != crouch){ //Hit
                if(player2->state == walkB){
                    destroyProjectile(&player1->projs, aux);
                    al_start_timer(player2->blockState);
                }
                else
                    hitApply(aux, NULL,player1, player2);
                aux = NULL;
            }

            if(aux){
                if(aux->direction == left)
                    aux->x -= aux->speed;
                else
                    aux->x += aux->speed;
            }
            aux = next;
        }
    }
    return;
}

//Controles
void keybinds(ALLEGRO_EVENT event, PLAYER* player1, PLAYER* player2){
    if(event.type == ALLEGRO_EVENT_KEY_DOWN){//Keybinds dos ataques
        //PLAYER1
        //Soco
        if(event.keyboard.keycode == ALLEGRO_KEY_R && 
                player1->state !=walkB &&
                player1->state != walkF &&
                !al_get_timer_started(player1->cooldownAttack) &&
                !al_get_timer_started(player1->damageState))
            attackWrapper(player1, player2, punch);
        //kick
        if(event.keyboard.keycode == ALLEGRO_KEY_T &&
                player1->state !=walkB && player1->state != walkF &&
                player1->state != crouch &&
                player1->directionY == none &&
                !al_get_timer_started(player1->cooldownAttackLow) &&
                !al_get_timer_started(player1->damageState))
            attackWrapper(player1, player2, kick);
        //Projetil
        if(event.keyboard.keycode == ALLEGRO_KEY_Y &&
                !al_get_timer_started(player1->cooldownProj) &&
                player1->state == stand &&
                !al_get_timer_started(player1->damageState))
            projectileWrapper(player1, player2);
        //PLAYER2
        if(event.keyboard.keycode == ALLEGRO_KEY_J &&
                player2->state !=walkB &&
                player2->state != walkF &&
                !al_get_timer_started(player2->cooldownAttack) &&
                !al_get_timer_started(player2->damageState))
            attackWrapper(player2, player1, punch);

        if(event.keyboard.keycode == ALLEGRO_KEY_K &&
                player2->state !=walkB &&
                player2->state != walkF &&
                player2->state != crouch &&
                player2->directionY == none &&
                !al_get_timer_started(player2->cooldownAttackLow) &&
                !al_get_timer_started(player2->damageState))
            attackWrapper(player2, player1, kick);

        if(event.keyboard.keycode == ALLEGRO_KEY_L &&
                !al_get_timer_started(player2->cooldownProj) &&
                player2->state == stand &&
                !al_get_timer_started(player2->damageState))
            projectileWrapper(player2, player1);
    }
    //TECLAS DE MOVIMENTACAO
    if(event.type == ALLEGRO_EVENT_KEY_DOWN || event.type == ALLEGRO_EVENT_KEY_UP){
        //PLAYER1
        if(event.keyboard.keycode == ALLEGRO_KEY_W)
            moveUp(&event, player1);
        if(event.keyboard.keycode == ALLEGRO_KEY_A)
            moveLeft(&event, player1, player2);
        if(event.keyboard.keycode == ALLEGRO_KEY_S)
            moveDown(&event, player1, player2);
        if(event.keyboard.keycode == ALLEGRO_KEY_D)
            moveRight(&event, player1, player2);
        //PLAYER2
        //se nao for controlado pelo computador
        if(!player2->bot){
            if(event.keyboard.keycode == ALLEGRO_KEY_UP)
                moveUp(&event, player2);
            if(event.keyboard.keycode == ALLEGRO_KEY_LEFT)
                moveLeft(&event, player2, player1);
            if(event.keyboard.keycode == ALLEGRO_KEY_DOWN)
                moveDown(&event, player2, player1);
            if(event.keyboard.keycode == ALLEGRO_KEY_RIGHT)
                moveRight(&event, player2, player1);
        }
    }
}
