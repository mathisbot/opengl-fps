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


/**
 * @brief Create a depth map
 * 
 * @param depthMap Pointer to the depth map
 * @param width Width of the depth map
 * @param height Height of the depth map
 * @return int 0 if success, -1 if error
*/
int createDepthCubemap(GLuint *depthMap, unsigned int resolution);

/**
 * @brief Bind a depth map
 * 
 * @param depthMapFBO Depth map FBO
 * @param depthMap Depth map to bind
*/
void bindDepthCubemapToFBO(GLuint depthMapFBO, GLuint depthMap);

/**
 * @brief Destroy a depth map
 * 
 * @param depthMap Depth map to destroy
*/
void destroyDepthCubemap(GLuint depthMap);


#endif