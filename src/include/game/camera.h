#ifndef CAMERA_H
#define CAMERA_H


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#include <SDL2/SDL.h>
#include <cglm/cglm.h>


#define EYE_Y 1.8f

#define GRAVITY 20.0f
#define JUMPSPEED 7.5f
#define SPEED 7.5f
#define SPRINTBOOST 1.75f
#define SENSITIVITY 0.13f
#define TERMINALVELOCITY 50.0f


/**
 * @brief Brindings structure
*/
typedef struct {
    SDL_Scancode forward;
    SDL_Scancode backward;
    SDL_Scancode left;
    SDL_Scancode right;
    SDL_Scancode sprint;
    SDL_Scancode jump;
    SDL_Scancode use;
    SDL_Scancode reload;
} Bindings;

/**
 * @brief Camera structure
 * 
 * @param pos Position of the camera
 * @param target Target of the camera
 * @param direction Direction of the camera
 * @param direction2D Direction of the camera projected on the xz plane
 * @param up Up vector of the camera
 * @param right Right vector of the camera
 * @param right2D Right vector of the camera projected on the xz plane
 * @param speed Speed of the camera
 * @param sprintBoost Boost applied to the speed when sprinting
 * @param sensitivity Sensitivity of the camera
 * @param onGround Whether the camera is on the ground
 * @param jumpSpeed Speed of the camera when jumping
 * @param upVelocity Up velocity of the camera
 * @param bindings Bindings of the camera
 * 
 * @note Direction is pos-target normalized (automatically calculated)
 * @note Direction2D is direction projected on the xz plane and normalized (automatically calculated)
 * @note Right is up*direction normalized (automatically calculated)
 * @note Right2D is up*direction2D normalized (automatically calculated)
 * @note Up is assumed to be {0.0f, 1.0f, 0.0f}. The reason here is that the camera is always upright,
 *       and that changing the up vector would require to compute arc-cosines and arc-sines at some point,
 *       which is not very efficient and not very useful in this type of game.
*/
typedef struct {
    vec3 pos;
    vec3 target;
    vec3 direction;
    vec3 direction2D;
    vec3 up;
    vec3 right;
    vec3 right2D;
    float speed;
    float sprintBoost;
    float sensitivity;
    bool onGround;
    float jumpSpeed;
    vec3 upVelocity;
    Bindings bindings;
} Camera;


/**
 * @brief Initialize a camera
 * 
 * @param camera Pointer to the camera to initialize
 * @param pos Position of the camera
 * @param target Target of the camera
 * @param bindings Bindings of the camera
 * @return int 0 on success, -1 on failure
 * 
 * @note Bindings are copied
*/
int initCamera(Camera *camera, vec3 pos, vec3 target, Bindings *bindings);

/**
 * @brief Translate the camera
 * 
 * @param camera Pointer to the camera to update
 * @param keyboardState Pointer to the keyboard state
 * @param dt Delta time
 * 
 * @note Should be called before rotateCamera
 * @note Assumes that ground is the (y=0) plane
 * @note Camera parameters are NOT updated
 * @note This only updates camera->pos and camera->target
*/
void translateCamera(Camera *camera, const Uint8* keyboardState, double dt);


/**
 * @brief Rotate the camera
 * 
 * @param camera Pointer to the camera to update
 * @param dx Delta x
 * @param dy Delta y
 * 
 * @note Should be called after translateCamera
 * @note Camera parameters are NOT updated
 * @note This only updates camera->target
*/
void rotateCamera(Camera *camera, int dx, int dy);


/**
 * @brief Update the camera
 * 
 * @param camera Pointer to the camera to update
 * 
 * @note Camera parameters are updated
*/
void updateCamera(Camera *camera);


#endif // CAMERA_H