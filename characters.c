#include "street.h"

//Cria Partida
MATCH* createMatch(){
    MATCH* match = NULL;

    match = (MATCH*)malloc(sizeof(MATCH));
    if(!match)
        return NULL;

    match->P1 = createPlayer(match ,MAX_X/4, GROUND_LEVEL+ BASE_HEIGHT / 2, BASE_LENGTH, BASE_HEIGHT);
    if(!match->P1)
        return NULL;

    match->P2 = createPlayer(match, MAX_X - MAX_X / 4, GROUND_LEVEL+ BASE_HEIGHT / 2, BASE_LENGTH, BASE_HEIGHT);
    if(!match->P2)
        return NULL;

    //FALTA MAPA
    match->dmgMult = 1;
    match->gravMult = 1;
    match->healthMult = 1;
    match->poiseMult = 1;
    match->speedMult = 1;
    match->staminaMult = 1;
    match->time = MATCH_LENGTH;

    return match;
}

// Cria o personagem se for valido sua posicao
PLAYER* createPlayer(MATCH* match, int x, int y, unsigned short length, unsigned short height){
    PLAYER* player = NULL;

    if(x > MAX_X || y > MAX_Y || x + length / 2 >MAX_X || x - length / 2 < 0 || y + height / 2 > MAX_Y || y - height / 2 < 0)
        return player;

    player =(PLAYER*)malloc(sizeof(PLAYER));
    if(!player)
        return NULL;

    //FALTA SPRITES E SONS
    player->x = x;
    player->y = y;
    player->height = height;
    player->length = length;
    player->health = BASE_HEALTH * match->healthMult;
    player->speedX = (MAX_X / 1000) * match->speedMult;
    player->speedY = (MAX_Y / 1000) * match->speedMult;
    player->poise = BASE_POISE * match->poiseMult;
    player->stamina = BASE_STAMINA * match->staminaMult;
    player->state = stand;
    player->directionX = none;
    player->directionY = none;
    player->gauge = 0;
    player->rounds = 0;
    player->stick = createJoystick();

    if(!player->stick){
        destroyPlayer(player);
        return NULL;
    }

    return player;
}

void destroyMatch(MATCH* match){

    destroyPlayer(match->P1);
    destroyPlayer(match->P2);
    fclose(match->map.map);
    fclose(match->map.soundMap);
}

void destroyPlayer(PLAYER* player){

    fclose(player->fighter.model);
    fclose(player->fighter.sounds);
    free(player->stick);
    free(player);
    return;
}

