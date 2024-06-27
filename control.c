#include "street.h"
#include <allegro5/bitmap_io.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

int countFilesFolder(char* path){

    int counter = 0;
    struct dirent *entry;
    DIR *dp = opendir(path);

    if (dp == NULL) {
        perror("opendir");
        return -1;
    }
    while ((entry = readdir(dp)))
        if (entry->d_type == DT_REG)
            counter++;
    closedir(dp);
    return counter;
}

void pressSpace(ALLEGRO_EVENT_QUEUE* queue, ALLEGRO_EVENT* event){
    while(1){
        al_wait_for_event(queue, event);
        if(event->type == ALLEGRO_EVENT_KEY_UP && event->keyboard.keycode == ALLEGRO_KEY_SPACE)
            break;
    }
    return;
}

void resetPlayer(PLAYER* player){
    player->health = BASE_HEALTH;
    player->stamina = BASE_STAMINA;
    player->state = stand;
    player->directionX = none;
    player->directionY = none;
    al_stop_timer(player->cooldownAttack);
    al_stop_timer(player->cooldownProj);
    al_stop_timer(player->damageState);
    al_stop_timer(player->attackDuration);
    return;
}

void roundReset(MATCH* match){
    match->time = MATCH_LENGTH;
    resetPlayer(match->P1);
    resetPlayer(match->P2);
    match->P1->x = MAX_X / 4.0;
    match->P1->y = GROUND_LEVEL - BASE_HEIGHT / 2;
    match->P2->x = MAX_X - MAX_X / 4.0;
    match->P2->y = GROUND_LEVEL - BASE_HEIGHT / 2;
    destroyList(&match->P1->projs);
    destroyList(&match->P2->projs);
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

//Aplica dano dependendo de oque fex o hit
void hitApply(PROJECTILE* projectile, ATTACK* attack,PLAYER* attacker, PLAYER* victim){
    if(projectile){
        victim->health -= projectile->dmg;
        destroyProjectile(&attacker->projs, projectile);
    }
    else if(attack){
        attack->hitFlag = true;
        victim->health -= attack->dmg;
    }
    al_start_timer(victim->damageState);
    return;
}

void checkHitAttack(PLAYER* attacker, PLAYER* victim){
    //Se nao houver ataque ou ja ter acertado retorna
    if(!attacker->attacks || attacker->attacks->hitFlag){
        return;
    }
    if(attacker->attacks->id == punch && inRangeX(attacker->attacks->x, victim) && inRangeY(attacker->attacks->y, victim) && victim->state != walkB && victim->state != crouch)//hit
        hitApply(NULL, attacker->attacks, attacker, victim);
    else if(attacker->attacks->id == kick && inRangeX(attacker->attacks->x, victim) && inRangeY(attacker->attacks->y, victim) && victim->state != crouch)
        hitApply(NULL, attacker->attacks, attacker, victim);
    return;
}

//Cooldowns do personagem
void cooldowns(ALLEGRO_EVENT event, PLAYER* player){
    if(event.timer.source == player->cooldownAttack)
        al_stop_timer(player->cooldownAttack);
    if(event.timer.source == player->cooldownProj)
        al_stop_timer(player->cooldownProj);
    if(event.timer.source == player->attackDuration){
        al_stop_timer(player->attackDuration);
        destroyAttack(player);
    }
    if(event.timer.source == player->damageState)
        al_stop_timer(player->damageState);
    return;
}

void staminaRegen(PLAYER* player){
    player->stamina += STAMINA_REGEN;
    player->stamina = (player->stamina > BASE_STAMINA) ? BASE_STAMINA : player->stamina;//Limite superior
    return;
}

//Recebe barra dependendo de oque aconteceu
void gaugeGain(PLAYER* player, short id){
    if(id == attack)
        player->gauge += ATTACK_GAUGE_GAIN;
    else if(id == projectile)
        player->gauge += PROJECTILE_GAUGE_GAIN;
    else
        player->gauge += DAMAGE_GAUGE_GAIN;
    player->gauge = (player->gauge > MAX_GAUGE) ? MAX_GAUGE : player->stamina; //Limite superior
    return;
}

//Seleciona qual animacoes vao ser feitas
void animationSelect(PLAYER* player){
    char path[1024];
    char* ch;
    int i;

    if(!player->fighter.newFlag){

        return;
    }

    if(player->fighter.sprite){
        for(i = 0; i < player->fighter.size; i++)
            al_destroy_bitmap(player->fighter.sprite[i]);
        free(player->fighter.sprite);
    }

    if(player->fighter.id == monk)
        strcpy(path,"images/fighters/monk/");
    else if(player->fighter.id == cleric)
        strcpy(path, "images/fighters/cleric/");

    //path para diretorio
    if(player->state == stand){
        if(!player->attacks)
            strcat(path,"idle/");
        else if(player->attacks->id == punch)
            strcat(path,"punch/");
        else if(player->attacks->id == kick)
            strcat(path,"kick/");
    }
    else if(player->state == walkF || player->state == walkB)
        strcat(path,"walk/");
    else if(player->state == jump || player->state == jumpB || player->state == jumpF)
        if(!player->attacks)
            strcat(path,"jump/");
        else
            strcat(path,"jumpAttack/");
    else if(player->state == crouch){
        if(!player->attacks)
            strcat(path,"crouch/");
        else
            strcat(path,"crouchAttack/");
    }


    //cria vetor de bitmaps
    player->fighter.sprite = createSprites(player, path);
    if(!player->fighter.sprite)
        return;
    strcat(path,"*.png");
    ch = strchr(path, '*');
    for(i = 0; i < player->fighter.size; i++){
        char c = i + 1 + '0'; //numero de i+1 em char
        *ch = c;
        printf("%s\n", path);
        player->fighter.sprite[i] = al_load_bitmap(path);
    }
    return;
}
