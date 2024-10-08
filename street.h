#ifndef _STREET_
#define _STREET_

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <stdbool.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/keyboard.h>
#include <allegro5/display.h>
#include <allegro5/events.h>
#include <allegro5/allegro_image.h>
#include <allegro5/mouse.h>
#include <dirent.h>

enum state{stand, walkF, walkB, crouch, jump, jumpF, jumpB, damage};
enum direction{none, up, down, left, right};
enum action{attack, hit, projectile};
enum attackType{punch, kick};
enum charID{monk, brawler, cleric};
enum menu{singlePlayer, multiplayer, quit};
enum pause{resume, mainMenu, exitGame};
enum map{forest,dojo, bamboo};

#define MAX_X 1280.0
#define MAX_Y 720.0
#define FPS 30.0

//Numeros base de elementos do jogo

#define MATCH_LENGTH 90
#define BASE_HEALTH 150
#define BASE_DMG_PUNCH 10
#define BASE_DMG_KICK 17
#define BASE_DMG_PROJ 15
#define BASE_STAMINA 120
#define MAX_GAUGE 100

#define BASE_SPEEDX (MAX_X / 150.0)
#define BASE_SPEEDY (MAX_Y / 25.0)
#define PROJECTILE_SPEED (BASE_SPEEDX * 2.5)
#define BASE_GRAV (MAX_Y/500)

#define HEADER_LEVEL (MAX_Y * 0.15)
#define FOOTER_LEVEL (MAX_Y * 0.8)
#define GROUND_LEVEL (MAX_Y*0.875) //nivel do chao

#define BASE_HEIGHT (MAX_Y*0.3)
#define BASE_LENGTH (MAX_X*0.1)
#define PROJ_SIZE (MAX_X*0.05)

#define BLOCK_DURATION 0.15
#define DAMAGE_DURATION 0.075
#define ATTACK_DURATION 0.375

#define PROJ_COOLDOWN 1.5
#define ATTACK_COOLDOWN 0.4

#define STAMINA_REGEN 1
#define PROJ_COST 50
#define PUNCH_COST 35
#define KICK_COST 45
#define BLOCK_COST 40

#define ATTACK_GAUGE_GAIN 5
#define DAMAGE_GAUGE_GAIN 3
#define PROJECTILE_GAUGE_GAIN 4

#define ATTACK_FRAME_DURATION 1/13.0 //frame de ataques
#define MOV_FRAME_DURATION 1/3.0 //frame de andar
#define GLOBAL_FRAME_TIME 1/20.0 //frame para animacoes de projeteis

#define AT_LEFT(X1,X2) ((X1) < (X2) ? true : false)
#define AT_RIGHT(X1,X2) ((X1) > (X2) ? true : false)

struct mapData{
    short size;
    ALLEGRO_BITMAP** map;
};

typedef struct attack{
    unsigned short id;
    unsigned short dmg;
    short direction;
    bool hitFlag;//flag de validacao do ataque 
    float x;//ponta do ataque
    float y;
    struct attack* next;
}ATTACK;

typedef struct projectile{
    float x;
    float y;
    float side;//Tamanho
    float speed;
    short direction;
    short currentFrame;
    unsigned int dmg;
    struct projectile* next;
}PROJECTILE;

typedef struct joystick{
    bool up;
    bool down;
    bool left;
    bool right;
    bool punch;
    bool kick;
    bool special;
    bool shoot;
    bool valUp;//flags de validade de input
    bool valDown;
    bool valLeft;
    bool valRight;
}JOYSTICK;

struct character{
    short id;
    int size;
    bool newFlag;
    short currentFrame;
    ALLEGRO_BITMAP** sprite;
    ALLEGRO_TIMER* frameMovement;
    ALLEGRO_TIMER* frameAttack;
};

typedef struct player{
    struct character fighter;
    JOYSTICK* stick;
    ATTACK* attacks;
    PROJECTILE* projs;
    float x;
    float y;
    float height;
    float length;
    float speedX;
    float speedY;
    unsigned short rounds;
    unsigned short directionX;
    unsigned short directionY;
    unsigned short state; //Stand, Crouch, Jump
    unsigned short gauge;
    short health;
    short stamina;
    int sizeSprites;
    bool bot;//true se computador controlar
    ALLEGRO_TIMER* cooldownProj;
    ALLEGRO_TIMER* cooldownAttack;
    ALLEGRO_TIMER* cooldownAttackLow;
    ALLEGRO_TIMER* attackDuration;
    ALLEGRO_TIMER* projDuration;
    ALLEGRO_TIMER* damageState;
    ALLEGRO_TIMER* blockState;
    ALLEGRO_BITMAP** spritesProjs;
}PLAYER;

struct uiData{
    ALLEGRO_BITMAP* bar;
    ALLEGRO_BITMAP* round;
};

typedef struct match{
    //ALLEGRO_SAMPLE* music;
    unsigned short time;
    ALLEGRO_TIMER_EVENT* frameTime;
    struct uiData ui;
    struct mapData map;
    struct player* P1;
    struct player* P2;
}MATCH;

void mustInit(bool test, char* description);
int countFilesFolder(char* folder_path);

void registerTimers(ALLEGRO_EVENT_QUEUE* queue, PLAYER* player);
void loadMap(MATCH* match, short map);
MATCH* createMatch(ALLEGRO_EVENT_QUEUE* queue, ALLEGRO_EVENT* event, ALLEGRO_FONT* font, bool bot);
PLAYER* createPlayer(MATCH* match,float x, float y, short id, bool bot);
JOYSTICK* createJoystick();
PROJECTILE* createProjectile(PLAYER* player, short direction, PROJECTILE* list);
ATTACK* createAttack(PLAYER* player, short id, short direction);
ALLEGRO_BITMAP** createSprites(PLAYER* player, char* folder);
ALLEGRO_BITMAP** createSpritesProjs(PLAYER* player, char* folder);

void destroyList(PROJECTILE** list);
void destroyPlayer(PLAYER* player);
void destroyAttack(PLAYER* player);
void destroyProjectile(PROJECTILE** list, PROJECTILE* p);
void destroyMatch(MATCH* match);

void attackWrapper(PLAYER* attacker, PLAYER* victim, short id);
void projectileWrapper(PLAYER* attacker, PLAYER* victim);

bool inRangeX(float x, PLAYER* player2);
bool inRangeY(float y, PLAYER* player2);

void moveUp(ALLEGRO_EVENT* event, PLAYER* player);
void moveLeft(ALLEGRO_EVENT* event, PLAYER* player, PLAYER* player2);
void moveDown(ALLEGRO_EVENT* event, PLAYER* player, PLAYER* player2);
void moveRight(ALLEGRO_EVENT* event, PLAYER* player, PLAYER* player2);

bool isMovementValidUp(PLAYER* player1, PLAYER* player2);
bool isMovementValidDown(PLAYER* player1, PLAYER* player2);
bool isMovementValidLeft(PLAYER* player1, PLAYER* player2);
bool isMovementValidRight(PLAYER* player1, PLAYER* player2);

void singlePlayerMovement(PLAYER* player1, PLAYER* player2);
void movePlayer(MATCH* match, PLAYER* player1, PLAYER* player2);
void moveProjectile(PLAYER* player1, PLAYER* player2);

void checkHitAttack(PLAYER* attacker, PLAYER* victim);
void hitApply(PROJECTILE* projectile, ATTACK* attack,PLAYER* attacker, PLAYER* victim);

void staminaRegen(PLAYER* player);

void roundReset(MATCH* match);
short pauseMenu(ALLEGRO_EVENT_QUEUE* queue,ALLEGRO_EVENT* event, ALLEGRO_FONT* font, MATCH* match);
void roundEnd(ALLEGRO_DISPLAY* disp,ALLEGRO_FONT* font ,MATCH* match, PLAYER* winner, PLAYER* winner2, ALLEGRO_EVENT_QUEUE* queue, ALLEGRO_EVENT event);
void pressSpace(ALLEGRO_EVENT_QUEUE* queue, ALLEGRO_EVENT* event);

void keybinds(ALLEGRO_EVENT event, PLAYER* player1, PLAYER* player2);
void cooldowns(ALLEGRO_EVENT event, PLAYER* player);
void animationSelect(PLAYER* player);
void animationSelectProjectile(PLAYER* player);

short drawMainMenu(ALLEGRO_EVENT_QUEUE* queue, ALLEGRO_EVENT* event, ALLEGRO_FONT* font);
void drawMap(MATCH* match);
void drawCharacter(PLAYER* player1, PLAYER* player2);
void drawProjectile(PLAYER* player);
void drawShadow(PLAYER* player);
short drawUI(ALLEGRO_DISPLAY* disp, ALLEGRO_FONT* font, ALLEGRO_EVENT_QUEUE* queue, ALLEGRO_EVENT event ,MATCH* match,PLAYER* player1, PLAYER* player2);
void drawRounds(PLAYER* player1, PLAYER* player2);
short selectFighter(ALLEGRO_EVENT_QUEUE* queue , ALLEGRO_EVENT* event, ALLEGRO_FONT* font, char* fighter);
short selectMap(ALLEGRO_EVENT_QUEUE* queue, ALLEGRO_EVENT* event, ALLEGRO_FONT* font);
void nextFrame(PLAYER* player, ALLEGRO_EVENT event);
void nextFrameProj(PLAYER* player);

#endif
