#ifndef ENEMY_H
#define ENEMY_H


#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>


typedef struct Enemy {
    uint32_t hp;
    float x;
    float y;
    float z;
    float yaw;
    float pitch;
    int type;
} Enemy;


/**
 * @brief Initialize an enemy
 * 
 * @param x X position of the enemy
 * @param y Y position of the enemy
 * @param z Z position of the enemy
 * @param yaw Yaw of the enemy
 * @param pitch Pitch of the enemy
 * @param type Type of the enemy
 * @return Enemy* Pointer to the enemy
*/
Enemy* createEnemy(float x, float y, float z, float yaw, float pitch, int type);

/**
 * @brief Free an enemy
 * 
 * @param enemy Pointer to the enemy
*/
void freeEnemy(Enemy* enemy);


/**
 * @brief Update an enemy
 * 
 * @param enemy Pointer to the enemy
*/
void updateEnemy(Enemy* enemy, double dt);

#endif
