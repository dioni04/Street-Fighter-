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

enum state{stand, crouch, jump};

#define MAX_X 1920
#define MAX_Y 1080
#define FPS 30.0

//Numeros base de elementos do jogo
#define MATCH_LENGHT 90
#define BASE_HEALTH 100
#define BASE_DMG 10
#define BASE_MOV_SPEED 25
#define BASE_POISE 100

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

typedef struct player{
    struct character fighter;
    unsigned short state; //Stand, Crouch, Jump
    unsigned short health;
    unsigned short poise;
    unsigned short speed;
    unsigned short gauge;
    unsigned short height;
    unsigned short width;
    unsigned short rounds;
    unsigned int x;
    unsigned int y;
}PLAYER;

typedef struct match{
    unsigned short time;
    unsigned short speedMult; //Multiplicadores para partida
    unsigned short dmgMult;
    struct mapData map;
    struct player P1;
    struct player P2;
}MATCH;

//Strutura do jogo
typedef struct game{
    struct gameData data;
}GAME;

#endif
