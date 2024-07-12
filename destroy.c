#include "street.h"
#include <allegro5/timer.h>

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

    destroyPlayer(match->P1);
    destroyPlayer(match->P2);
    /*
    fclose(match->map.map);
    fclose(match->map.soundMap);
    */
    //al_destroy_sample(match->music);
    for(int i = 0; i < match->map.size; i++)
        al_destroy_bitmap(match->map.map[i]);
    // al_destroy_audio_stream(match->map.music);
    free(match);
}

void destroyPlayer(PLAYER* player){
    free(player->stick);
    destroyList(&player->projs);
    al_destroy_timer(player->cooldownAttack);
    al_destroy_timer(player->cooldownProj);
    al_destroy_timer(player->attackDuration);
    al_destroy_timer(player->damageState);
    al_destroy_timer(player->fighter.frameAttack);
    al_destroy_timer(player->fighter.frameMovement);
    al_destroy_timer(player->projDuration);
    for(int i = 0; i < player->sizeSprites; i++)
        al_destroy_bitmap(player->spritesProjs[i]);
    free(player);
    return;
}
