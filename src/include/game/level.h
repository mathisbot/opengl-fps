#ifndef LEVEL_H
#define LEVEL_H


#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
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

/* Level file structure:

start: startX startY startZ startYaw startPitch
enemies: x y z yaw pitch type
health: healthcount
x y z
ammo: ammocount
x y z
textures: texturecount
relativePathToTexture
wall: wallcount
texture: textureID
pointcount: pointcount
tex_x tex_y x y z  // top left
tex_x tex_y x y z  // bottom left
tex_x tex_y x y z  // top right
tex_x tex_y x y z  // bottom right
...

Walls are drawn as triangles, so pointcount that are over 3
are split into multiple triangles (n-2 triangles for n points)
as each new triangle is defined by the first point and the last two points
*/


/**
 * @brief Create a point
 * 
 * @param x X position of the point
 * @param y Y position of the point
 * @param z Z position of the point
 * @param tex_x X position of the texture
 * @param tex_y Y position of the texture
 * @return Point* Pointer to the point
 */
Point* createPoint(float x, float y, float z, float tex_x, float tex_y);

/**
 * @brief Free a point
 * 
 * @param point Pointer to the point
 */
void freePoint(Point* point);


/**
 * @brief Create a wall
 * 
 * @param points Array of points (vertices) of the wall
 * @param pointCount Number of points (vertices) of the wall
 * @param textureID ID of the texture of the wall
 * @return Wall* Pointer to the wall
 */
Wall* createWall(Point** points, int pointCount, GLuint textureID);

/**
 * @brief Free a wall
 * 
 * @param wall Pointer to the wall
 */
void freeWall(Wall* wall);


/**
 * @brief Create a level by reading a file
 * 
 * @param levelNumber Number of the level
 * @return Level* Pointer to the level
 */
Level* loadLevel(uint16_t levelNumber);

/**
 * @brief Free a level
 * 
 * @param level Pointer to the level
 */
void freeLevel(Level* level);

#endif