#include "player.h"


Player* createPlayer()
{
    Player* player = malloc(sizeof(Player));
    if (!player)
    {
        printf("Error allocating memory for player\n");
        exit(1);
    }

    player->speed = SPEED;
    player->sensitivity = SENSITIVITY;
    player->hp = HP;
    player->currentLevel = 1;

    return player;
}

void freePlayer(Player* player)
{
    free(player);
}
