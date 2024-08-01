#include "street.h"

/*
 * ARQUIVO DE FREEs DE STRUCTS E TIMERS
 */

//Libera projetil e relinka a lista
void destroyProjectile(PROJECTILE** list, PROJECTILE* p){
    PROJECTILE* aux = *list;
    if (!list || !*list || !p) return;
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

void destroyAttack(PLAYER* player){
    if(!player->attacks)
        return;
    free(player->attacks);
    player->attacks = NULL;
    return;
}

void destroyList(PROJECTILE** list){
    PROJECTILE* aux;

    while(*list){
        aux = (*list)->next;
        free(*list);
        *list = aux;
    }
    list = NULL;
    return;
}

void destroyMatch(MATCH* match){
    if(!match)
        return; 
    destroyPlayer(match->P1);
    destroyPlayer(match->P2);
    for(int i = 0; i < match->map.size; i++)
        al_destroy_bitmap(match->map.map[i]);
    free(match);
    match = NULL;
    return; 
}

void destroyPlayer(PLAYER* player){
    if(!player)
        return; 
    free(player->stick);
    destroyList(&player->projs);
    al_destroy_timer(player->cooldownAttack);
    al_destroy_timer(player->cooldownAttackLow);
    al_destroy_timer(player->cooldownProj);
    al_destroy_timer(player->attackDuration);
    al_destroy_timer(player->damageState);
    al_destroy_timer(player->blockState);
    al_destroy_timer(player->projDuration);
    al_destroy_timer(player->fighter.frameAttack);
    al_destroy_timer(player->fighter.frameMovement);
    for(int i = 0; i < player->sizeSprites; i++)
        al_destroy_bitmap(player->spritesProjs[i]);
    for(int i = 0; i < player->fighter.size; i++)
        al_destroy_bitmap(player->fighter.sprite[i]);
    free(player->fighter.sprite);
    
    free(player);
    player = NULL;
    return;
}
