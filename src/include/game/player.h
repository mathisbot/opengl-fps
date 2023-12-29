#ifndef PLAYER_H
#define PLAYER_H


#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>


#define HP 100.0f


typedef struct {
    uint16_t hp;
    uint16_t currentLevel;
} Player;


/**
 * @brief Create a player
 * 
 * @return Player* Pointer to the player
*/
Player* createPlayer(uint16_t hp, uint16_t currentLevel);

/**
 * @brief Free a player
 * 
 * @param player Pointer to the player
*/
void freePlayer(Player* player);


/**
 * @brief Update the player
 * 
 * @param player Pointer to the player
 * @param dt Time since last update
*/
void updatePlayer(Player* player, double dt);

#endif