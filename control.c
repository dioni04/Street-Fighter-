#include "street.h"

void mustInit(bool test, char* description){
    if(!test){
        printf("%s\n", description);
        exit(1);
    }
    return;
}

int countFilesFolder(char* path){
    int counter = 0;
    struct dirent *dir;
    DIR *dp = opendir(path);

    if (dp == NULL)
        return -1;
    while ((dir = readdir(dp)))
        if(dir->d_type == DT_REG)
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
    player->fighter.newFlag = true;
    destroyAttack(player);
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
    if(event.timer.source == player->cooldownAttack){
        al_stop_timer(player->cooldownAttack);
    }
    if(event.timer.source == player->cooldownProj){
        al_stop_timer(player->cooldownProj);
    }
    if(event.timer.source == player->attackDuration){
        al_stop_timer(player->attackDuration);
        destroyAttack(player);
        player->fighter.newFlag = true;
    }
    if(event.timer.source == player->damageState){
        al_stop_timer(player->damageState);
        player->fighter.newFlag = true;
    }
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

//Carrega sprites dos projeteis
void animationSelectProjectile(PLAYER* player){
    char path[1024];
    char* ch;

    if(player->fighter.id == monk)
        strcpy(path,"images/projectiles/blue/");
    else if (player->fighter.id == cleric || player->fighter.id == brawler)
        strcpy(path,"images/projectiles/red/");

    player->spritesProjs = createSpritesProjs(player, path);
    strcat(path,"*.png");
    ch = strchr(path, '*');
    for(int i = 0; i < player->sizeSprites; i++){
        char c = i + 1 + '0'; //numero de i+1 em char
        *ch = c;//carrega sprites nomeados na ordem ASCII a partir de 1
        printf("%s\n", path);
        player->spritesProjs[i] = al_load_bitmap(path);
    }
    return;
}

//Seleciona qual animacoes vao ser feitas
void animationSelect(PLAYER* player){
    char path[1024];
    char* ch;
    int i;
    bool attack = false;

    if(!player->fighter.newFlag)
        return;

    if(player->fighter.sprite){
        for(i = 0; i < player->fighter.size; i++)
            al_destroy_bitmap(player->fighter.sprite[i]);
        free(player->fighter.sprite);
        player->fighter.sprite = NULL;
    }

    //Seleciona fighter
    if(player->fighter.id == monk)
        strcpy(path,"images/fighters/monk/");
    else if(player->fighter.id == cleric)
        strcpy(path, "images/fighters/cleric/");
    else if(player->fighter.id == brawler)
        strcpy(path, "images/fighters/brawler/");

    //Path para diretorio
    if(player->state == stand){
        if(!player->attacks)
            strcat(path,"idle/");
        else if(player->attacks->id == punch){
            strcat(path,"punch/");
            attack = true;
        }
        else if(player->attacks->id == kick){
            strcat(path,"kick/");
            attack = true;
        }
    }
    else if(player->state == walkF || player->state == walkB)
        strcat(path,"walk/");
    else if(player->state == jump || player->state == jumpB || player->state == jumpF)
        if(!player->attacks)
            strcat(path,"jump/");
        else{
            strcat(path,"jumpAttack/");
            attack = true;
        }
    else if(player->state == crouch){
        if(!player->attacks)
            strcat(path,"crouch/");
        else{
            strcat(path,"crouchAttack/");
            attack = true;
        }
    }

    //Para timers de frames de ataques e movimentos dependendo da acao atual
    if(attack){
        al_stop_timer(player->fighter.frameMovement);
        if(!al_get_timer_started(player->fighter.frameAttack))
            al_start_timer(player->fighter.frameAttack);
    }
    else{
        al_stop_timer(player->fighter.frameAttack);
        if(!al_get_timer_started(player->fighter.frameMovement))
            al_start_timer(player->fighter.frameMovement);
    }

    //cria vetor de bitmaps
    player->fighter.sprite = createSprites(player, path);
    if(!player->fighter.sprite)
        return;
    strcat(path,"*.png");
    ch = strchr(path, '*');
    for(i = 0; i < player->fighter.size; i++){
        char c = i + 1 + '0'; //numero de i+1 em char
        *ch = c;//carrega sprites nomeados na ordem ASCII a partir de 1
        // printf("%s\n", path);
        player->fighter.sprite[i] = al_load_bitmap(path);
    }
    return;
}
