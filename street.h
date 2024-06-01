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

#define MAX_X 1920
#define MAX_Y 1080
#define FPS 30.0

//Numeros base de elementos do jogo
#define MATCH_LENGTH 90
#define BASE_HEALTH 100
#define BASE_DMG 10
#define BASE_POISE 100
#define BASE_STAMINA 100
#define BASE_GRAV 10
#define GROUND_LEVEL (MAX_Y / 1000) //nivel do chao
#define BASE_HEIGHT (MAX_Y / 5)
#define BASE_LENGTH (MAX_X / 10)

#define BACK_MOV_LEFT(X1,X2) ((X1) < (X2) ? true : false)
#define BACK_MOV_RIGHT(X1,X2) ((X1) > (X2) ? true : false)

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

typedef struct joystick{
    bool up;
    bool down;
    bool left;
    bool right;
}JOYSTICK;

typedef struct player{
    struct character fighter;
    JOYSTICK* stick;
    unsigned short state; //Stand, Crouch, Jump
    unsigned short height;
    unsigned short length;
    unsigned short rounds;
    unsigned short directionX;
    unsigned short directionY;
    unsigned short speedX;
    unsigned short speedY;
    unsigned short health;
    unsigned short poise;
    unsigned short stamina;
    unsigned short gauge;
    unsigned int x;
    unsigned int y;
}PLAYER;

typedef struct match{
    unsigned short time;
    unsigned short speedMult; //Multiplicadores para partida
    unsigned short dmgMult;
    unsigned short gravMult;
    unsigned short poiseMult;
    unsigned short healthMult;
    unsigned short staminaMult;
    struct mapData map;
    struct player* P1;
    struct player* P2;
}MATCH;

//Strutura do jogo
typedef struct game{
    struct gameData data;
    MATCH* match;
}GAME;


PLAYER* createPlayer(MATCH* match,int x, int y, unsigned short length, unsigned short height);
JOYSTICK* createJoystick();
void destroyPlayer(PLAYER* player);
bool backMovementLeft(PLAYER* player1, PLAYER* player2);
bool backMovementRight(PLAYER* player1, PLAYER* player2);

bool isMovementValidUp(PLAYER* player1, PLAYER* player2);
bool isMovementValidDown(PLAYER* player1, PLAYER* player2);
bool isMovementValidLeft(PLAYER* player1, PLAYER* player2);
bool isMovementValidRight(PLAYER* player1, PLAYER* player2);

#endif
