#ifndef LEVEL_H
#define LEVEL_H

#include <stdio.h>
#include <stdlib.h>

typedef struct {
    float startX;
    float startY;
    float startZ;
    float startYaw;
    float startPitch;
    float* walls;
    int wallCount;
} Level;

Level* loadLevel(char* path);
void freeLevel(Level* level);

#endif