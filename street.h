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
#include <allegro5/keyboard.h>
#include <allegro5/display.h>
#include <allegro5/events.h>

enum state{stand, walkF, walkB, crouch, jump, jumpF, jumpB, damage};
enum direction{none, up, down, left, right};
enum action{no, attack, block};
enum attackType{punch, kick};

#define MAX_X 640.0
#define MAX_Y 480.0
#define FPS 30.0

//Numeros base de elementos do jogo
#define MATCH_LENGTH 90
#define BASE_HEALTH 100
#define BASE_DMG_PUNCH 15
#define BASE_DMG_KICK 25
#define BASE_DMG_PROJ 30
#define BASE_POISE 100
#define BASE_STAMINA 100
#define BASE_SPEEDX (MAX_X / 150.0)
#define BASE_SPEEDY (MAX_Y / 25.0)
#define PROJECTILE_SPEED (BASE_SPEEDX * 2.5)
#define BASE_GRAV (MAX_Y/500)
#define HEADER_LEVEL (MAX_Y * 0.15)
#define FOOTER_LEVEL (MAX_Y * 0.85)
#define GROUND_LEVEL (MAX_Y*0.8) //nivel do chao

#define BASE_HEIGHT (MAX_Y*0.3)
#define BASE_LENGTH (MAX_X*0.1)
#define PROJ_SIZE (MAX_X*0.05)

#define PROJ_COOLDOWN 1
#define ATTACK_COOLDOWN 0.5

#define STAMINA_REGEN 1
#define PROJ_COST 20

#define AT_LEFT(X1,X2) ((X1) < (X2) ? true : false)
#define AT_RIGHT(X1,X2) ((X1) > (X2) ? true : false)

//Vetores com os assets do jogo
struct gameData{
    FILE** models;
    FILE** charSounds;
    FILE** map;
    FILE** mapSounds;
};

struct character{
    FILE* model;
    FILE* sounds;
};

struct mapData{
    FILE* map;
    FILE* soundMap;
};

typedef struct attack{
    unsigned short id;
    unsigned short dmg;
    short direction;
    float x;//PONTA DO ATAQUE
    float y;
    struct attack* next;
}ATTACK;

typedef struct projectile{
    float x;
    float y;
    float side;//Tamanho
    short direction;
    float speed;
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
}JOYSTICK;

typedef struct player{
    struct character fighter;
    JOYSTICK* stick;
    ATTACK* attacks;
    PROJECTILE* projs;
    unsigned short state; //Stand, Crouch, Jump
    float x;
    float y;
    float height;
    float length;
    float speedX;
    float speedY;
    unsigned short rounds;
    unsigned short directionX;
    unsigned short directionY;
    short health;
    short poise;
    short stamina;
    ALLEGRO_TIMER* cooldownProj;
    ALLEGRO_TIMER* cooldownAttack;
    unsigned short gauge;

}PLAYER;

typedef struct match{
    unsigned short time;
    float speedMult; //Multiplicadores para partida
    float dmgMult;
    float gravMult;
    float poiseMult;
    float healthMult;
    float staminaMult;
    struct mapData map;
    struct player* P1;
    struct player* P2;
}MATCH;

//Strutura do jogo
typedef struct game{
    struct gameData data;
    MATCH* match;
}GAME;

void moveUp(PLAYER* player);
void moveLeft(PLAYER* player, PLAYER* player2);
void moveDown(PLAYER* player, PLAYER* player2);
void moveRight(PLAYER* player, PLAYER* player2);

//KEYBINDS* createKeys();
MATCH* createMatch();
PLAYER* createPlayer(MATCH* match,float x, float y);
JOYSTICK* createJoystick();
PROJECTILE* createProjectile(PLAYER* player, short direction, PROJECTILE* list);

void destroyPlayer(PLAYER* player);
bool backMovementLeft(PLAYER* player1, PLAYER* player2);
bool backMovementRight(PLAYER* player1, PLAYER* player2);

bool isMovementValidUp(PLAYER* player1, PLAYER* player2);
bool isMovementValidDown(PLAYER* player1, PLAYER* player2);
bool isMovementValidLeft(PLAYER* player1, PLAYER* player2);
bool isMovementValidRight(PLAYER* player1, PLAYER* player2);

void movePlayer(MATCH* match, PLAYER* player1, PLAYER* player2);
void moveProjectile(PLAYER* player1, PLAYER* player2);

void destroyMatch(MATCH* match);
void destroyProjectile(PROJECTILE** list, PROJECTILE* p);

void roundEnd(ALLEGRO_DISPLAY* disp,ALLEGRO_FONT* font ,MATCH* match, PLAYER* winner, PLAYER* winner2);
void pressSpace(ALLEGRO_EVENT_QUEUE* queue, ALLEGRO_EVENT* event);

ATTACK* createAttack(PLAYER* player, short id, short direction);
void destroyAttack(PLAYER* player);

#endif
