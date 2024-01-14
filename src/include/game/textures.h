#ifndef TEXTURES_H
#define TEXTURES_H


#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>

#include "logs.h"


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
    char path[512];
} Texture;

typedef struct {
    GLuint id;
    int width;
    int height;
    char path[512];
} Cubemap;


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


/**
 * @brief Load a cubemap
 * 
 * @param cubemap Pointer to the cubemap
 * @param path Path to the cubemap, must be relative to the TEXTUREPATH
 * @param extension Extension of the cubemap
 * @return int 0 if success, -1 if error
 * 
 * @note Path must be relative to the TEXTUREPATH
*/
int loadCubemap(Cubemap *cubemap, char* path, char* extension);

/**
 * @brief Load a cubemap
 * 
 * @param cubemap Pointer to the cubemap
 * @param path Path to the cubemap, must be absolute
 * @param extension Extension of the cubemap
 * @return int 0 if success, -1 if error
 * 
 * @note Path must be absolute
*/
int loadCubemapFullPath(Cubemap *cubemap, char *fullpath, char* extension);

/**
 * @brief Load a cubemap
 * 
 * @param cubemap Pointer to the cubemap
 * @param path Path to the cubemap, must be absolute
 * @param numMipmaps Number of mipmaps to generate
 * @return int 0 if success, -1 if error
 * 
 * @note Path must be absolute
*/
void destroyCubemap(Cubemap *cubemap);


#endif