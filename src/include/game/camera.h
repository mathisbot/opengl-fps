#ifndef CAMERA_H
#define CAMERA_H

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include <SDL2/SDL.h>


// This will be rounded depending on float precision
#define PI 3.14159265358979323846f
#define DEG2RAD(_d) ((_d) * (PI / 180.0f))

#define GRAVITY 20.0f
#define JUMPSPEED 10.0f
#define SPEED 10.0f
#define SPRINTINGBOOST 1.5f
#define SENSITIVITY 17.5f
#define DOUBLEJUMP 0


typedef struct {
    SDL_Scancode forward;
    SDL_Scancode backward;
    SDL_Scancode left;
    SDL_Scancode right;
    SDL_Scancode sprint;
    SDL_Scancode jump;
    SDL_Scancode use;
    SDL_Scancode reload;
    SDL_Scancode inventory;
    SDL_Scancode map;
    SDL_Scancode pause;
} Bindings;

typedef struct {
    float x; 
    float y;
    float z;
    float yaw;  // Degrees
    float pitch;  // Degrees
    float movingSpeed;
    float sprintingBoost;
    float rotationSpeed;
    float xVelocity;
    float yVelocity;
    float zVelocity;
    Bindings* bindings;
    bool onGround;
    bool canDoubleJump;
    bool hasDoubleJump;

    // Used for optimization
    // These quantities are used multiple times each frames
    float yawCos;  // Cosine of yaw
    float yawSin;  // Sine of yaw
    float pitchCos;  // Cosine of pitch
    float pitchSin;  // Sine of pitch  
    float movCos;; // camera->movingSpeed * camera->yawCos * dt
    float movSin;; // camera->movingSpeed * camera->yawSin * dt
} Camera;


/**
 * @brief Create a Bindings object
 * 
 * @param forward Key to move forward
 * @param backward Key to move backward
 * @param left Key to move forward
 * @param right Key to move right
 * @param sprint Key to move left
 * @param jump Key to jump
 * @param use Key to jump
 * @param reload Key to reload
 * @param inventory Key to open inventory
 * @param map Key to open the map
 * @param pause Key to pause the game
 * @return Bindings* Pointer to bindings
*/
Bindings* createBindings(SDL_Scancode forward, SDL_Scancode backward, SDL_Scancode left, SDL_Scancode right, SDL_Scancode sprint, SDL_Scancode jump,
                        SDL_Scancode use, SDL_Scancode reload, SDL_Scancode inventory, SDL_Scancode map, SDL_Scancode pause);

/**
 * @brief Free bindings
 * 
 * @param bindings Pointer to bindings
*/
void freeBindings(Bindings* bindings);



/**
 * @brief Initialize a camera
 * 
 * @param x X position of the camera
 * @param y Y position of the camera
 * @param z Z position of the camera
 * @param yaw Yaw of the camera
 * @param pitch Pitch of the camera
 * @param movingSpeed Speed at which the camera moves
 * @param rotationSpeed Speed at which the camera rotates
 * @param bindings Bindings of the camera
 * @return Camera* Pointer to the camera
*/
Camera* initCamera(float x, float y, float z, float yaw, float pitch, float movingSpeed, float sprintingBoost, float rotationSpeed, Bindings* bindings, bool canDoubleJump);

/**
 * @brief Free a camera
 * 
 * @param camera Pointer to the camera
*/
void freeCamera(Camera* camera);


/**
 * @brief Update the camera position
 * 
 * @param camera Pointer to the camera
 * @param keyboardState Pointer to keyboard state
 * @param dt Time since last frame in seconds
 * @param speedMultiplier Coefficient multiplying forward movement (e.g. sprinting)
*/
void handleCameraMovement(Camera* camera, const Uint8* keyboadState, double dt, float speedMultiplier);

/**
 * @brief Make the camera jump
 * 
 * @param camera Pointer to the camera
*/
void cameraJump(Camera* camera);

/**
 * @brief Update the camera rotation
 * 
 * @param camera Pointer to the camera
 * @param motionX Mouse motion on the X axis
 * @param motionY Mouse motion on the Y axis
 * @param dt Time since last frame
*/
void handleCameraRotation(Camera* camera, float motionX, float motionY, double dt);

#endif