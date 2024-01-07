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

#define TEXTURE_DIFFUSE 0
#define TEXTURE_SPECULAR 1
#define TEXTURE_NORMAL 2
#define TEXTURE_HEIGHT 3

typedef struct {
    GLuint id;
    int width;
    int height;
    uint8_t type;
    char path[256];
} Texture;


/**
 * @brief Load a texture
 *   
 * @param tex Pointer to the texture
 * @param path Path to the texture, must be relative to the TEXTUREPATH
 * @param numMipmaps Number of mipmaps to generate
 * @param repeat Repeat the texture
 * @param type Type of the texture
 * @return int 0 if success, -1 if error
 * 
 * @note Path must be relative to the TEXTUREPATH
*/
int loadTexture(Texture *tex, const char* path, int numMipmaps, bool repeat, uint8_t type);

/**
 * @brief Load a texture
 *   
 * @param tex Pointer to the texture
 * @param path Path to the texture, must be absolute
 * @param numMipmaps Number of mipmaps to generate
 * @param repeat Repeat the texture
 * @param type Type of the texture
 * @return int 0 if success, -1 if error
 * 
 * @note Path must be absolute
*/
int loadTextureFullPath(Texture *tex, const char* path, int numMipmaps, bool repeat, uint8_t type);

/**
 * @brief Destroy a texture
 * 
 * @param tex Texture to destroy
*/
void destroyTexture(Texture tex);


#endif