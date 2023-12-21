#ifndef PLAYER_H
#define PLAYER_H

#include <stdlib.h>
#include <stdio.h>

typedef struct {
    float speed;
    float sensitivity;
    float hp;
} Player;

Player* createPlayer(int speed, int sensitivity, float hp);
void freePlayer(Player* player);

#endif