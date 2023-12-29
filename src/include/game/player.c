#include "player.h"


Player* createPlayer(uint16_t hp, uint16_t currentLevel)
{
    Player* player = malloc(sizeof(Player));
    if (!player)
    {
        fprintf(stderr, "Error allocating memory for player\n");
        return NULL;
    }

    player->hp = hp;
    player->currentLevel = currentLevel;

    return player;
}

void freePlayer(Player* player)
{
    free(player);
}


void updatePlayer(Player* player, double dt)
{
    if (player->hp <= 0.0f)
    {
        // Kill player
        return;
    }
    // If enemy close, take damage
    return;
}