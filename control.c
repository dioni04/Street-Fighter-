#include "street.h"


JOYSTICK* createJoystick(){
    JOYSTICK* stick = NULL;

    stick = (JOYSTICK*)malloc(sizeof(JOYSTICK));
    if(!stick)
        return NULL;
    stick->up = false;
    stick->down = false;
    stick->left = false;
    stick->right = false;

    return stick;
}

bool inRangeX(PLAYER* player1, PLAYER* player2){
    if(player1->x <= player2->x + (player2->length / 2) && player1->x >= player2->x - (player2->length / 2))
        return true;
    return false;
}

bool inRangeY(PLAYER* player1, PLAYER* player2){
    if(player1->y <= player2->y + (player2->height / 2) && player1->y >= player2->y - (player2->height / 2))
        return true;
    return false;
}

void move_up(PLAYER* player){

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

void move_down(PLAYER* player){

    player->stick->down = !player->stick->down;

    //Se estiver pulando nao faz nada
    if(player->stick->down){
        if(player->state == stand || player->state == walkB || player->state == walkF){
            player->state = crouch;
            player->directionX = none;
        }
    }
    else if(!player->stick->down && player->state == crouch)
        player->state = stand;

    return;
}

void move_left(PLAYER* player1, PLAYER* player2){

    player1->stick->left = !player1->stick->left;

    //Dependendo da posicao do outro player recebe estado
    if(player1->stick->left && player1->state == stand){
        if(BACK_MOV_LEFT(player1->x, player2->x))
            player1->state = walkB;
        else
            player1->state = walkF;
        player1->directionX = left;
    }
    //Se soltar e estar andando para esquerda para de andar
    else if(!player1->stick->left && (player1->state == walkB || player1->state == walkF) && !player1->stick->right){
        player1->state = stand;
        player1->directionX = none;
    }

    return;
}

void move_right(PLAYER* player1, PLAYER* player2){

    player1->stick->right = !player1->stick->right;

    //Dependendo da posicao do outro player recebe estado
    if(player1->stick->right && player1->state == stand){
        if(BACK_MOV_RIGHT(player1->x, player2->x))
            player1->state = walkB;
        else
            player1->state = walkF;
        player1->directionX = right;
    }
     //Se soltar e estar andando para esquerda para de andar
    if(!player1->stick->right && (player1->state == walkB || player1->state == walkF) && !player1->stick->left){
        player1->state = stand;
        player1->directionX = none;
    }

    return;
}

//Calcula se movimento é valido no eixo Y
bool isMovementValidUp(PLAYER* player1, PLAYER* player2){
    //Se tiver movimento para cima
    if(player1->y + (player1->height / 2) + player1->speedY > MAX_Y)
        return false;
    //Se estiver em baixo do outro player
    if(player1->y < player2->y && inRangeX(player1, player2))
        if(player1->y + (player1->height / 2) + player1->speedY > (player2->y - player2->height / 2))
            return false;
    return true;
}

//Calcula se movimento é valido no eixo Y
bool isMovementValidDown(PLAYER* player1, PLAYER* player2){

    if((player1->y - (player1->height / 2) - player1->speedY) < GROUND_LEVEL)
        return false;
    //Se tiver encima do outro player
    if(player1->y > player2->y && inRangeX(player1, player2))
        if(player1->y - (player1->height / 2) - player1->speedY < (player2->y + player2->height / 2))
            return false;
    return true;
}

//Testa se movemento nao quebra nenhum limite
bool isMovementValidLeft(PLAYER* player1, PLAYER* player2){

    //Se tiver movimento para cima
    if(player1->directionY == up ){
        if(!isMovementValidUp(player1, player2))
            return false;
    }
    //Se tiver movimento para baixo
    else if(player1->directionY == down)
        if(!isMovementValidDown(player1, player2))
            return false;

    if(player1->x - (player1->length / 2) - player1->speedX < 0)
        return false;
    //Se estiver na direita e no intervalo da altura do outro player
    if(player1->x > player2->x && inRangeY(player1, player2)){
        if(player1->x - (player1->length / 2) - player1->speedX < player2->x + player2->length / 2)
            return false;
    }
    return true;
}

bool isMovementValidRight(PLAYER* player1, PLAYER* player2){

    //Se tiver movimento para cima
    if(player1->directionY == up){
        if(!isMovementValidUp(player1, player2))
            return false;
    }
    //Se tiver movimento para baixo
    else if(player1->directionY == down)
        if(!isMovementValidDown(player1,player2))
            return false;

    if(player1->x + (player1->length / 2) + player1->speedX > MAX_X)
        return false;
    //Se outro player estar na esquerda e no intervalo da altura do outro player
    if(player1->x < player2->x && inRangeY(player1, player2)){
        if(player1->x + (player1->length / 2) + player1->speedX > player2->x - player2->length / 2)
            return false;
    }
    return true;
}

