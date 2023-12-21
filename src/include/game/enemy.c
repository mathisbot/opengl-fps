#include "enemy.h"


Enemy* create_enemy()
{
    Enemy* enemy = malloc(sizeof(Enemy));
    if (!enemy)
    {
        printf("Error allocating memory for enemy\n");
        exit(1);
    }

    enemy->hp = 100.0f;
    enemy->x = 0.0f;
    enemy->y = 0.0f;
    enemy->z = 0.0f;

    return enemy;
}

void destroy_enemy(Enemy* enemy)
{
    free(enemy);
}
