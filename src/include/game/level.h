#ifndef LEVEL_H
#define LEVEL_H

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <stdint.h>


#define LEVELCOUNT 1
#define LEVELPATH "assets/levels/"


typedef struct {
    float startX;
    float startY;
    float startZ;
    float startYaw;
    float startPitch;
    float* walls;
    int wallCount;
} Level;

Level* loadLevel(uint16_t levelNumber);
void freeLevel(Level* level);

#endif