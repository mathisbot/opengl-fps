#include "player.h"

Player* createPlayer(int speed, int sensitivity, float hp)
{
    Player* player = malloc(sizeof(Player));
    if (!player)
    {
        printf("Error allocating memory for player\n");
        exit(1);
    }

    player->speed = speed;
    player->sensitivity = sensitivity;
    player->hp = hp;

    return player;
}

void freePlayer(Player* player)
{
    free(player);
}
