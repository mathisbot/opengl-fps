#ifndef TEXTURES_H
#define TEXTURES_H


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <GL/glu.h>


typedef struct {
    char path[256];
    GLuint id;
    int width;
    int height;
} Texture;


Texture* loadTexture(const char* path);
void freeTexture(Texture* texture);


#endif