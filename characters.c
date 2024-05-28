#include "street.h"

// Cria o personagem se for valido sua posicao
PLAYER* createPlayer(int x, int y, unsigned short width, unsigned short height){
    PLAYER* player = NULL;

    if(x > MAX_X || y > MAX_Y || x + width / 2 >MAX_X || x - width / 2 < 0 || y + height / 2 > MAX_Y || y - height / 2 < 0)
        return player;

    player = malloc(sizeof(PLAYER));
    if(!player)
        return NULL;

    //FALTA SPRITES E SONS
    player->x = x;
    player->y = y;
    player->height = height;
    player->width = width;
    player->health = BASE_HEALTH;
    player->speed = BASE_MOV_SPEED;
    player->poise = BASE_POISE;
    player->state = stand;
    player->gauge = 0;
    player->rounds = 0;

    return player;
}

void destroyPlayer(PLAYER* player){

    fclose(player->fighter.model);
    fclose(player->fighter.sounds);
    free(player);

    return;
}
