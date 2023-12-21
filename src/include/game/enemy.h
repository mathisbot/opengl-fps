#ifndef ENEMY_H
#define ENEMY_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

// Basic enemy struct
typedef struct Enemy {
    uint32_t hp;
    float x;
    float y;
    float z;
} Enemy;

Enemy* create_enemy();
void destroy_enemy(Enemy* enemy);

#endif
