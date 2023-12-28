#include "camera.h"


Bindings* createBindings(SDL_Scancode forward, SDL_Scancode backward, SDL_Scancode left, SDL_Scancode right, SDL_Scancode sprint, SDL_Scancode jump,
                         SDL_Scancode use, SDL_Scancode reload, SDL_Scancode inventory, SDL_Scancode map, SDL_Scancode pause)
{
    Bindings* bindings = (Bindings*)malloc(sizeof(Bindings));
    if (!bindings)
    {
        printf("Error allocating memory for bindings\n");
        return NULL;
    }

    bindings->forward = forward;
    bindings->backward = backward;
    bindings->left = left;
    bindings->right = right;
    bindings->sprint = sprint;
    bindings->jump = jump;
    bindings->use = use;
    bindings->reload = reload;
    bindings->inventory = inventory;
    bindings->map = map;
    bindings->pause = pause;

    return bindings;
}

void freeBindings(Bindings* bindings)
{
    free(bindings);
}


Camera* initCamera(float x, float y, float z, float yaw, float pitch, float movingSpeed, float sprintingBoost, float rotationSpeed, Bindings* bindings, bool canDoubleJump)
{
    Camera* camera = malloc(sizeof(Camera));
    if (!camera)
    {
        printf("Error allocating memory for camera\n");
        return NULL;
    }

    camera->x = x;
    camera->y = y;
    camera->z = z;
    camera->yaw = yaw;
    camera->yawCos = cos(DEG2RAD(yaw));
    camera->yawSin = sin(DEG2RAD(yaw));
    camera->pitch = pitch;
    camera->pitchCos = cos(DEG2RAD(pitch));
    camera->pitchSin = sin(DEG2RAD(pitch));
    camera->movCos = 0.0f;
    camera->movSin = 0.0f;
    camera->movingSpeed = movingSpeed;
    camera->sprintingBoost = sprintingBoost;
    camera->rotationSpeed = rotationSpeed;
    camera->xVelocity = 0.0f;
    camera->yVelocity = 0.0f;
    camera->zVelocity = 0.0f;
    camera->bindings = bindings;
    camera->onGround = 1;
    camera->canDoubleJump = canDoubleJump;
    camera->hasDoubleJump = 1;

    return camera;
}

void freeCamera(Camera* camera)
{
    freeBindings(camera->bindings);
    free(camera);
}


void handleCameraMovement(Camera* camera, const Uint8* keyboardState, double dt, float speedMultiplier)
{
    // Updating movement factor
    camera->movCos = camera->movingSpeed * camera->yawCos * dt;
    camera->movSin = camera->movingSpeed * camera->yawSin * dt;

    // (x,z) plane movement
    if (keyboardState[camera->bindings->forward])  // Forward
    {
        camera->x += camera->movSin * speedMultiplier;
        camera->z -= camera->movCos * speedMultiplier;
    }
    if (keyboardState[camera->bindings->backward])  // Backward
    {
        camera->x -= camera->movSin;
        camera->z += camera->movCos;
    }
    if (keyboardState[camera->bindings->left])  // Left
    {
        camera->x -= camera->movCos;
        camera->z -= camera->movSin;
    }
    if (keyboardState[camera->bindings->right])  // Right
    {
        camera->x += camera->movCos;
        camera->z += camera->movSin;
    }

    // y axis movement
    if (!camera->onGround)  // Fall
    {
        camera->yVelocity -= GRAVITY * dt;
        camera->y += camera->yVelocity * dt;
        // I am currently assuming that ground is at y = 0
        // This is very likely to change in the future
        if (camera->y < 0.0f)  // Check if on ground
        {
            camera->onGround = 1;
            camera->yVelocity = 0.0f;
            camera->y = 0.0f;
        }
    }
}

void cameraJump(Camera* camera)
{
    // Handle jump
    if (camera->onGround)
    {
        camera->yVelocity += JUMPSPEED;
        camera->onGround = 0;
    }
}

void handleCameraRotation(Camera* camera, float motionX, float motionY, double dt)
{
    // fmod is used to prevent yaw from getting too large
    // thus keeping precision in the float
    camera->yaw += fmod(motionX * camera->rotationSpeed, 360.0f);
    camera->yawCos = cos(DEG2RAD(camera->yaw));
    camera->yawSin = sin(DEG2RAD(camera->yaw));

    camera->pitch -= motionY * camera->rotationSpeed;  
    // 89 degrees is the limit for pitch
    // This prevents the camera from flipping over
    if (camera->pitch > 89)
        camera->pitch = 89;
    else if (camera->pitch < -89)
        camera->pitch = -89;
    camera->pitchCos = cos(DEG2RAD(camera->pitch));
    camera->pitchSin = sin(DEG2RAD(camera->pitch));
}