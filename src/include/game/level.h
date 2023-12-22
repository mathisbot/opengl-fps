#ifndef LEVEL_H
#define LEVEL_H

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <dirent.h>
#include <stdint.h>

#include "enemy.h"
#include "textures.h"


#define LEVELCOUNT 1
#define LEVELPATH "assets/levels/"


typedef struct {
    float x;
    float y;
    float z;
    float tex_x;
    float tex_y;
} Point;

// Walls are defined by points (vertices)
typedef struct {
    Point** points;
    int pointCount;
    GLuint textureID;
} Wall;


typedef struct {
    float startX;
    float startY;
    float startZ;
    float startYaw;
    float startPitch;
    Enemy** enemies;
    int enemyCount;
    Wall** walls;
    int wallCount;
    Texture** textures;
    int textureCount;
} Level;

// Level file structure:
//
// start: startX startY startZ startYaw startPitch
// enemies: x y z yaw pitch type
// health: healthcount
// x y z
// ammo: ammocount
// x y z
// textures: texturecount
// path
// wall: wallcount
// texture: textureID
// pointcount: pointcount
// tex_x tex_y x y z
// tex_x tex_y x y z
// tex_x tex_y x y z
// tex_x tex_y x y z
// ...


Point* createPoint(float x, float y, float z, float tex_x, float tex_y);
void freePoint(Point* point);

Wall* createWall(Point** points, int pointCount, GLuint textureID);
void freeWall(Wall* wall);


Level* loadLevel(uint16_t levelNumber);
void freeLevel(Level* level);

#endif