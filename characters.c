#include "street.h"
#include <time.h>

//Cria Partida
MATCH* createMatch(){
    MATCH* match = NULL;

    match = (MATCH*)malloc(sizeof(MATCH));
    if(!match)
        return NULL;

    match->P1 = createPlayer(match ,MAX_X/4.0, GROUND_LEVEL - BASE_HEIGHT / 2);
    if(!match->P1)
        return NULL;

    match->P2 = createPlayer(match, MAX_X - MAX_X / 4.0, GROUND_LEVEL - BASE_HEIGHT / 2);
    if(!match->P2)
        return NULL;

    //FALTA MAPA
    match->dmgMult = 1.0;
    match->gravMult = 1.0;
    match->healthMult = 1.0;
    match->poiseMult = 1.0;
    match->speedMult = 1.0;
    match->staminaMult = 1.0;
    match->time = MATCH_LENGTH;

    return match;
}

// Cria o personagem se for valido sua posicao
PLAYER* createPlayer(MATCH* match, float x, float y){
    PLAYER* player = NULL;

    if(x > MAX_X || y > MAX_Y || x + BASE_LENGTH / 2 > MAX_X || x - BASE_LENGTH / 2 < 0 || y + BASE_HEIGHT / 2 > MAX_Y || y - BASE_HEIGHT / 2 < 0)
        return player;

    player =(PLAYER*)malloc(sizeof(PLAYER));
    if(!player)
        return NULL;

    //FALTA SPRITES E SONS
    player->x = x;
    player->y = y;
    player->height = BASE_HEIGHT;
    player->length = BASE_LENGTH;
    player->health = BASE_HEALTH;// * match->healthMult;
    player->speedX = BASE_SPEEDX;// * match->speedMult;
    player->speedY = BASE_SPEEDY;// * match->speedMult;
    player->poise = BASE_POISE;// * match->poiseMult;
    player->stamina = BASE_STAMINA;// * match->staminaMult;
    player->state = stand;
    player->directionX = none;
    player->directionY = none;
    player->gauge = 0;
    player->rounds = 0;
    player->stick = createJoystick();
    player->projs = NULL;
    player->attacks = NULL;
    //player->keys = createKeys();

    if(!player->stick){
        destroyPlayer(player);
        return NULL;
    }

    return player;
}

JOYSTICK* createJoystick(){
    JOYSTICK* stick = NULL;

    stick = (JOYSTICK*)malloc(sizeof(JOYSTICK));
    if(!stick)
        return NULL;
    stick->up = false;
    stick->down = false;
    stick->left = false;
    stick->right = false;
    stick->kick = false;
    stick->punch = false;
    stick->special = false;
    stick->shoot = false;

    return stick;
}

PROJECTILE* createProjectile(PLAYER* player, short direction, PROJECTILE* list){
    PROJECTILE* proj = NULL;

    proj = (PROJECTILE*)malloc(sizeof(PROJECTILE));
    if(!proj)
        return NULL;
    proj->direction = direction;
    proj->dmg = BASE_DMG;
    proj->speed = PROJECTILE_SPEED;
    proj->y = player->y;
    if(direction == left)
        proj->x = player->x - player->length / 2;
    else
        proj->x = player->x + player->length / 2;
    proj->side = PROJ_SIZE;
    proj->next = list;
    return proj;
}

//Libera projetil e relinka a lista
void destroyProjectile(PROJECTILE** list, PROJECTILE* p){
    PROJECTILE* aux = *list;
    if(*list == p){
        *list = (*list)->next;
        free(p);
        return;
    }
    while(aux->next != p)
        aux = aux->next;
    aux->next = p->next;
    free(p);
    return;
}


void destroyMatch(MATCH* match){

    destroyPlayer(match->P1);
    destroyPlayer(match->P2);
    /*
    fclose(match->map.map);
    fclose(match->map.soundMap);
    */
    free(match);
}

void destroyPlayer(PLAYER* player){

    /*
    fclose(player->fighter.model);
    fclose(player->fighter.sounds);
    */
    //free(player->keys);
    //FALTA DESTROIR LISTAS
    free(player->stick);
    free(player);
    return;
}

