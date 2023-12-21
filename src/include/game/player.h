#ifndef PLAYER_H
#define PLAYER_H

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>


#define HP 100.0f
#define SPEED 10.0f
#define SENSITIVITY 17.5f


typedef struct {
    float speed;
    float sensitivity;
    uint16_t hp;
    uint16_t currentLevel;
} Player;

Player* createPlayer();
void freePlayer(Player* player);

#endif