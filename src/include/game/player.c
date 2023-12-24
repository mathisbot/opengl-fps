#include "player.h"


Player* createPlayer(uint16_t hp, uint16_t currentLevel)
{
    Player* player = malloc(sizeof(Player));
    if (!player)
    {
        printf("Error allocating memory for player\n");
        exit(1);
    }

    player->hp = hp;
    player->currentLevel = currentLevel;

    return player;
}

void freePlayer(Player* player)
{
    free(player);
}
