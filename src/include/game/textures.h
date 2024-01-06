#ifndef TEXTURES_H
#define TEXTURES_H


#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>


#define TEXTUREPATH "assets/textures/"
#define TEXTURE_REPEAT 1
#define TEXTURE_CLAMP 0


typedef struct {
    GLuint id;
    int width;
    int height;
} Texture;


/**
 * @brief Load a texture
 *   
 * @param tex Pointer to the texture
 * @param path Path to the texture, must be relative to the TEXTUREPATH
 * @param numMipmaps Number of mipmaps to generate
 * @param repeat Repeat the texture
 * @return int 0 if success, -1 if error
*/
int loadTexture(Texture *tex, const char* path, int numMipmaps, bool repeat);

/**
 * @brief Destroy a texture
 * 
 * @param tex Texture to destroy
*/
void destroyTexture(Texture tex);


#endif