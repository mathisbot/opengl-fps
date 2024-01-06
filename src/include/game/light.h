#ifndef LIGHT_H
#define LIGHT_H


#define SHADOWMAP_RES 1024
#define SHADOWMAP_ZNEAR 0.1f
#define SHADOWMAP_ZFAR 32.0f


#include <stdio.h>
#include <stdlib.h>

#include <SDL2/SDL.h>
#include <cglm/cglm.h>

#include "textures.h"  // For the cubemap texture


typedef struct {
    vec3 position;
    vec3 color;
    GLuint depthCubemap;
} PointLight;


/**
 * @brief Initialize a point light
 * 
 * @param light The point light to initialize
 * @param position The position of the light
 * @param color The color of the light
 * @return int 0 if success, -1 if error
 * 
 * @note The depth cubemap is automatically allocated
*/
int initPointLight(PointLight *light, vec3 position, vec3 color);

/**
 * @brief Destroy a point light
 * 
 * @param light The point light to destroy
*/
void destroyPointLight(PointLight *light);


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
 * 
 * @note FBO gets unboud after the function call
*/
void bindDepthCubemapToFBO(GLuint depthMapFBO, GLuint depthMap);

/**
 * @brief Bind a point light to a FBO
 * 
 * @param light Point light to bind
 * @param depthMapFBO FBO to bind the light to
 * 
 * @note FBO gets unboud after the function call
*/
void bindPointLightToFBO(GLuint depthMapFBO, PointLight *light);

/**
 * @brief Get the 6 projection matrices of a point light for depth cubemapping
 * 
 * @param pointLight Point light to get the projection matrices from
 * @param lightProjection Projection matrix of the light
 * @param dest Destination array of matrices
 * 
 * @note The destination array must be of size 6
*/
void pointLightGetProjMatrices(PointLight *pointLight, mat4 *lightProjection, mat4 (*dest)[6]);

/**
 * @brief Destroy a depth map
 * 
 * @param depthMap Depth map to destroy
*/
void destroyDepthCubemap(GLuint depthMap);


#endif