#include "street.h"
#include <time.h>

//Cria Partida
MATCH* createMatch(){
    MATCH* match = NULL;

    match = (MATCH*)malloc(sizeof(MATCH));
    if(!match)
        return NULL;

    //match->music = al_load_sample("last_v8.mod");
    //must_init(match->music, "last_v8.mod");
    //al_play_sample( match->music, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
    //match->music =

    /*
    match->map.music = al_load_audio_stream("./last_v8.mod", 4, 2048);
    al_set_audio_stream_playing(match->map.music, true);
    al_set_audio_stream_gain(match->map.music, 1.0);
    */
    match->map.map = (ALLEGRO_BITMAP**)malloc(sizeof(match->map.map) * 4);
    for(int i = 0; i < 4; i++){
        char c = i + 1 + '0'; //numero de i+1 em char
        char location[25] = "images/map/forest/*.png";
        location[18] = c;
        match->map.map[i] = al_load_bitmap(location);
    }

    match->ui.staminaBar = al_load_bitmap("images/ui/stamina.png");
    if(!match->ui.staminaBar)
        printf("Stamina Error");
    match->ui.healthBar = al_load_bitmap("images/ui/health.png");
    if(!match->ui.healthBar)
        printf("Health Error");
    if(!match->map.map)
        return NULL;
    match->P1 = createPlayer(match ,MAX_X/4.0, GROUND_LEVEL - BASE_HEIGHT / 2, monk);
    if(!match->P1)
        return NULL;

    match->P2 = createPlayer(match, MAX_X - MAX_X / 4.0, GROUND_LEVEL - BASE_HEIGHT / 2, brawler);
    if(!match->P2)
        return NULL;

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
PLAYER* createPlayer(MATCH* match, float x, float y, short id){
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
    player->cooldownProj = 0;
    player->stick = createJoystick();
    player->projs = NULL;
    player->attacks = NULL;
    player->fighter.newFlag = true;
    player->fighter.id = id;//temp
    player->fighter.size = 0;
    player->fighter.sprite = NULL;
    player->spritesProjs = NULL;

    animationSelectProjectile(player);

    //TIMERS COOLDOWN
    player->cooldownProj = al_create_timer(PROJ_COOLDOWN);
    if(!player->cooldownProj)
        return NULL;
    player->cooldownAttack = al_create_timer(ATTACK_COOLDOWN);
    if(!player->cooldownAttack)
        return NULL;
    player->attackDuration = al_create_timer(ATTACK_DURATION);
    if(!player->attackDuration)
        return NULL;
    player->damageState = al_create_timer(DAMAGE_DURATION);
    if(!player->damageState)
        return NULL;
    player->fighter.frameMovement = al_create_timer(MOV_FRAME_DURATION);
    if(!player->fighter.frameMovement)
        return NULL;
    player->fighter.frameAttack = al_create_timer(ATTACK_FRAME_DURATION);
    if(!player->fighter.frameAttack)
        return NULL;
    //JOYSTICK
    if(!player->stick){
        destroyPlayer(player);
        return NULL;
    }
    return player;
}

ALLEGRO_BITMAP** createSpritesProjs(PLAYER* player, char* folder){
    ALLEGRO_BITMAP** sprites = NULL;
    player->sizeSprites = countFilesFolder(folder);
    if(player->sizeSprites <= 0)
        return NULL;
    sprites = (ALLEGRO_BITMAP**)malloc(sizeof(ALLEGRO_BITMAP*)* player->sizeSprites);
    mustInit(sprites != NULL, "sprites");
    return sprites;
}

ALLEGRO_BITMAP** createSprites(PLAYER* player, char* folder){
    ALLEGRO_BITMAP** sprites = NULL;
    player->fighter.size = countFilesFolder(folder);
    player->fighter.currentFrame = 0;
    player->fighter.newFlag = false;
    if(player->fighter.size <= 0)
        return NULL;
    sprites = (ALLEGRO_BITMAP**)malloc(sizeof(ALLEGRO_BITMAP*)* player->fighter.size);
    mustInit(sprites != NULL, "sprites");
    return sprites;
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



ATTACK* createAttack(PLAYER* player, short id, short direction){
    ATTACK* attack = NULL;

    attack = (ATTACK*)malloc(sizeof(ATTACK));
    if(!attack)
        return NULL;
    attack->direction = direction;
    if(id == punch){//soco
        attack->dmg = BASE_DMG_PUNCH;
        attack->y = player->y;
    }
    else{//chute
        attack->dmg = BASE_DMG_KICK;
        attack->y = player->y + player->height / 2;
    }
    attack->id = id;
    if(direction == left)
        attack->x = player->x - player->length * 1.25;
    else
        attack->x = player->x + player->length * 1.25;
    attack->hitFlag = false;
    al_start_timer(player->cooldownAttack);
    return attack;
}

PROJECTILE* createProjectile(PLAYER* player, short direction, PROJECTILE* list){
    PROJECTILE* proj = NULL;

    proj = (PROJECTILE*)malloc(sizeof(PROJECTILE));
    if(!proj)
        return NULL;
    proj->direction = direction;
    proj->dmg = BASE_DMG_PROJ;
    proj->speed = PROJECTILE_SPEED;
    proj->y = player->y;
    proj->currentFrame = 0;
    if(direction == left)
        proj->x = player->x - player->length / 2;
    else
        proj->x = player->x + player->length / 2;
    proj->side = PROJ_SIZE;
    proj->next = list;
    return proj;
}

void attackWrapper(PLAYER* attacker, PLAYER* victim, short id){
    if(AT_LEFT(attacker->x, victim->x))
        attacker->attacks = createAttack(attacker, id,right);
    else
        attacker->attacks = createAttack(attacker, id,left);
    al_start_timer(attacker->attackDuration);
    al_start_timer(attacker->cooldownAttack);
    if(id == punch)
        attacker->stamina -= PUNCH_COST;
    else
        attacker->stamina -= KICK_COST;
    attacker->fighter.newFlag = true;
    return;
}

void projectileWrapper(PLAYER* attacker, PLAYER* victim){
    if(AT_LEFT(attacker->x, victim->x))
        attacker->projs = createProjectile(attacker, right, attacker->projs);
    else
        attacker->projs = createProjectile(attacker, left, attacker->projs);
    al_start_timer(attacker->cooldownProj);
    attacker->stamina -= PROJ_COST;
    return;
}
