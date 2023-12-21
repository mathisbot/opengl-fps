#ifndef CAMERA_H
#define CAMERA_H

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include <SDL2/SDL.h>


#include <SDL2/SDL.h>


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
    float rotationSpeed;
    float xVelocity;
    float yVelocity;
    float zVelocity;
    Bindings bindings;
    bool onGround;
} Camera;

Camera* initCamera(float x, float y, float z, float yaw, float pitch, float movingSpeed, float rotationSpeed, Bindings bindings);
void freeCamera(Camera* camera);
void handleCameraMovement(Camera* camera, const Uint8* keyboardState, double dt);
void handleCameraRotation(Camera* camera, float motionX, float motionY, double dt);

#endif