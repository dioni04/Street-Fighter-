#include "street.h"

//Cria Partida
MATCH* createMatch(ALLEGRO_EVENT_QUEUE* queue, ALLEGRO_EVENT* event, ALLEGRO_FONT* font, bool bot){
    MATCH* match = NULL;

    match = (MATCH*)malloc(sizeof(MATCH));
    if(!match)
        return NULL;

    loadMap(match, selectMap(queue, event, font));
    match->ui.bar = al_load_bitmap("images/ui/EmptyBar.png");
    mustInit(match->ui.bar != NULL, "Bar Error");
    mustInit(match->map.map != NULL, "Map Error");
    match->P1 = createPlayer(match ,MAX_X/4.0, GROUND_LEVEL - BASE_HEIGHT / 2, selectFighter(queue, event, font, "FIGHTER 1"), false);
    mustInit(match->P1 != NULL, "Player 1 Error");
    match->P2 = createPlayer(match, MAX_X - MAX_X / 4.0, GROUND_LEVEL - BASE_HEIGHT / 2, selectFighter(queue, event, font, "FIGHTER 2"), bot);
    mustInit(match->P2 != NULL, "Player 2 Error");
    match->time = MATCH_LENGTH;
    return match;
}

// Cria o personagem se for valido sua posicao
PLAYER* createPlayer(MATCH* match, float x, float y, short id, bool bot){
    PLAYER* player = NULL;

    if(x > MAX_X || y > MAX_Y || x + BASE_LENGTH / 2 > MAX_X || x - BASE_LENGTH / 2 < 0 || y + BASE_HEIGHT / 2 > MAX_Y || y - BASE_HEIGHT / 2 < 0)
        return player;

    player =(PLAYER*)malloc(sizeof(PLAYER));
    if(!player)
        return NULL;

    player->x = x;
    player->y = y;
    player->height = BASE_HEIGHT;
    player->length = BASE_LENGTH;
    player->health = BASE_HEALTH;
    if(!bot)
        player->speedX = BASE_SPEEDX;
    else 
        player->speedX = BASE_SPEEDX * 0.925;//bot é um pouco mais lento para evitar problema de colisão
    player->speedY = BASE_SPEEDY;
    player->stamina = BASE_STAMINA;
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
    player->fighter.id = id;
    player->fighter.size = 0;
    player->fighter.sprite = NULL;
    player->spritesProjs = NULL;
    player->bot = bot;
    animationSelectProjectile(player);

    //TIMERS COOLDOWN
    player->cooldownProj = al_create_timer(PROJ_COOLDOWN);
    mustInit(player->cooldownProj != NULL, "Proj Cooldown");
    if(!bot)
        player->cooldownAttack = al_create_timer(ATTACK_COOLDOWN);
    else
        player->cooldownAttack = al_create_timer(ATTACK_COOLDOWN * 3.25);
    mustInit(player->cooldownAttack != NULL, "Attack Cooldown");
    if(!bot)
        player->cooldownAttackLow = al_create_timer(ATTACK_COOLDOWN);
    else
        player->cooldownAttackLow = al_create_timer(ATTACK_COOLDOWN * 4.25);
    mustInit(player->cooldownAttackLow != NULL, "Cooldown Attack Low");
    player->attackDuration = al_create_timer(ATTACK_DURATION);
    mustInit(player->attackDuration != NULL, "Attack Duration");
    player->damageState = al_create_timer(DAMAGE_DURATION);
    mustInit(player->damageState != NULL, "Damage Duration");
    player->blockState = al_create_timer(BLOCK_DURATION);
    mustInit(player->blockState != NULL, "Block Duration");
    player->fighter.frameMovement = al_create_timer(MOV_FRAME_DURATION);
    mustInit(player->fighter.frameMovement != NULL, "Frame Movemnet");
    player->fighter.frameAttack = al_create_timer(ATTACK_FRAME_DURATION);
    mustInit(player->fighter.frameAttack != NULL, "Attack Frame Duration");
    player->projDuration = al_create_timer(ATTACK_DURATION);
    mustInit(player->projDuration != NULL, "Proj Duration");

    //JOYSTICK
    if(!player->stick){
        destroyPlayer(player);
        return NULL;
    }
    return player;
}

//Registra timers de player
void registerTimers(ALLEGRO_EVENT_QUEUE* queue, PLAYER* player){
    al_register_event_source(queue, al_get_timer_event_source(player->cooldownProj));
    al_register_event_source(queue, al_get_timer_event_source(player->cooldownAttack));
    al_register_event_source(queue, al_get_timer_event_source(player->cooldownAttackLow));
    al_register_event_source(queue, al_get_timer_event_source(player->attackDuration));
    al_register_event_source(queue, al_get_timer_event_source(player->damageState));
    al_register_event_source(queue, al_get_timer_event_source(player->fighter.frameMovement));
    al_register_event_source(queue, al_get_timer_event_source(player->fighter.frameAttack));
    al_register_event_source(queue, al_get_timer_event_source(player->blockState));
    al_register_event_source(queue, al_get_timer_event_source(player->projDuration));
    return;
}

//conta arquivos e cria vetor de bitmaps
ALLEGRO_BITMAP** createSpritesProjs(PLAYER* player, char* folder){
    ALLEGRO_BITMAP** sprites = NULL;
    player->sizeSprites = countFilesFolder(folder);
    if(player->sizeSprites <= 0)
        return NULL;
    sprites = (ALLEGRO_BITMAP**)malloc(sizeof(ALLEGRO_BITMAP*)* player->sizeSprites);
    mustInit(sprites != NULL, "sprites");
    return sprites;
}

//conta arquivos e cria vetor de bitmaps
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

//cria joystick com flags de validacao
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
    stick->valUp = false;
    stick->valDown = false;
    stick->valLeft = false;
    stick->valRight = false;

    return stick;
}

//cria ataque
ATTACK* createAttack(PLAYER* player, short id, short direction){
    ATTACK* attack = NULL;

    attack = (ATTACK*)malloc(sizeof(ATTACK));
    mustInit(attack != NULL, "Attack Error");
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
    return attack;
}

//cria projetil
PROJECTILE* createProjectile(PLAYER* player, short direction, PROJECTILE* list){
    PROJECTILE* proj = NULL;

    proj = (PROJECTILE*)malloc(sizeof(PROJECTILE));
    mustInit(proj != NULL, "Proj Error");
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

//carrega bitmaps do cenario
void loadMap(MATCH* match, short map){
    char path[512];
    if(map == forest)
        strcpy(path, "images/map/forest/");
    else if(map == dojo)
        strcpy(path, "images/map/dojo/");
    else if(map == bamboo)
        strcpy(path, "images/map/bamboo/");
    match->map.size = countFilesFolder(path);
    match->map.map = (ALLEGRO_BITMAP**)malloc(sizeof(match->map.map) * match->map.size);
    mustInit(match->map.map != NULL, "Map");

    strcat(path, "*.png");
    char* ch = strchr(path, '*');
    for(int i = 0; i < match->map.size; i++){
        char c = i + 1 + '0'; //numero de i+1 em char
        *ch = c;
        match->map.map[i] = al_load_bitmap(path);
    }
    return ;
}

/*Wrapper para ataques
 * cria flags
 * cooldowns e timers
 * stamina
 */
void attackWrapper(PLAYER* attacker, PLAYER* victim, short id){
    //sem stamina
    if(id == punch && attacker->stamina < PUNCH_COST)
        return;
    else if(id == kick && attacker->stamina < KICK_COST)
        return;

    if(AT_LEFT(attacker->x, victim->x))
        attacker->attacks = createAttack(attacker, id,right);
    else
        attacker->attacks = createAttack(attacker, id,left);
    if(!al_get_timer_started(attacker->attackDuration))
        al_start_timer(attacker->attackDuration);
    else{//animation cancel
        al_stop_timer(attacker->attackDuration);
        al_start_timer(attacker->attackDuration);
    }
    if(id == punch){
        attacker->stamina -= PUNCH_COST;
        al_start_timer(attacker->cooldownAttack);
    }
    else{
        attacker->stamina -= KICK_COST;
        al_start_timer(attacker->cooldownAttackLow);
    } 
    attacker->fighter.newFlag = true;
    return;
}

/*Wrapper para projeteis
 * cria flags
 * cooldowns e timers
 * stamina
 */
void projectileWrapper(PLAYER* attacker, PLAYER* victim){
    if(attacker->stamina < PROJ_COST)
        return;
    if(AT_LEFT(attacker->x, victim->x))
        attacker->projs = createProjectile(attacker, right, attacker->projs);
    else
        attacker->projs = createProjectile(attacker, left, attacker->projs);
    al_start_timer(attacker->projDuration);
    al_start_timer(attacker->cooldownProj);
    attacker->fighter.newFlag = true;
    attacker->stamina -= PROJ_COST;
    return;
}
