#include "enemy.h"


Enemy* createEnemy(float x, float y, float z, float yaw, float pitch, int type)
{
    Enemy* enemy = malloc(sizeof(Enemy));
    if (!enemy)
    {
        printf("Error allocating memory for enemy\n");
        return NULL;
    }

    enemy->hp = 100.0f;
    enemy->x = x;
    enemy->y = y;
    enemy->z = z;
    enemy->yaw = yaw;
    enemy->pitch = pitch;
    enemy->type = type;

    return enemy;
}

void freeEnemy(Enemy* enemy)
{
    free(enemy);
}
