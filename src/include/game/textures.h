#ifndef TEXTURES_H
#define TEXTURES_H


#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>


#define TEXTUREPATH "assets/textures/"
#define TEXTURE_REPEAT 1


typedef struct {
    char path[256];
    GLuint id;
    int width;
    int height;
} Texture;


/**
 * @brief Load a texture
 *   
 * @param path Path to the texture, must be relative to the TEXTUREPATH
 * @param repeat Repeat the texture
 * @return Texture* Pointer to the texture 
*/
Texture* loadTexture(const char* path, bool repeat);

/**
 * @brief Free a texture
 * 
 * @param texture Pointer to the texture
*/
void freeTexture(Texture* texture);


#endif