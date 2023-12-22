#include "camera.h"


#ifndef PI
    #define PI 3.14159265358979323846f
    #define DEG2RAD(_d) ((_d) * (PI / 180.0f))
#endif

#define GRAVITY 20.0f
#define JUMPSPEED 10.0f


Camera* initCamera(float x, float y, float z, float yaw, float pitch, float movingSpeed, float rotationSpeed, Bindings bindings)
{
    Camera* camera = malloc(sizeof(Camera));
    if (!camera)
    {
        printf("Error allocating memory for camera\n");
        exit(1);
    }

    camera->x = x;
    camera->y = y;
    camera->z = z;
    camera->yaw = yaw;
    camera->pitch = pitch;
    camera->movingSpeed = movingSpeed;
    camera->rotationSpeed = rotationSpeed;
    camera->xVelocity = 0.0f;
    camera->yVelocity = 0.0f;
    camera->zVelocity = 0.0f;
    camera->bindings = bindings;
    camera->onGround = 1;

    return camera;
}

void freeCamera(Camera* camera)
{
    free(camera);
}

void handleCameraMovement(Camera* camera, const Uint8* keyboardState, double dt)
{
    // (x,z) plane movement
    if (keyboardState[camera->bindings.forward])  // Forward
    {
        float coeff = 1.0f;
        // Check if sprinting
        if (keyboardState[camera->bindings.sprint])
            coeff = 1.5f;
        camera->x += camera->movingSpeed * sin(DEG2RAD(camera->yaw)) * dt * coeff;
        camera->z -= camera->movingSpeed * cos(DEG2RAD(camera->yaw)) * dt * coeff;
    }
    if (keyboardState[camera->bindings.backward])  // Backward
    {
        camera->x -= camera->movingSpeed * sin(DEG2RAD(camera->yaw)) * dt;
        camera->z += camera->movingSpeed * cos(DEG2RAD(camera->yaw)) * dt;
    }
    if (keyboardState[camera->bindings.left])  // Left
    {
        camera->x -= camera->movingSpeed * cos(DEG2RAD(camera->yaw)) * dt;
        camera->z -= camera->movingSpeed * sin(DEG2RAD(camera->yaw)) * dt;
    }
    if (keyboardState[camera->bindings.right])  // Right
    {
        camera->x += camera->movingSpeed * cos(DEG2RAD(camera->yaw)) * dt;
        camera->z += camera->movingSpeed * sin(DEG2RAD(camera->yaw)) * dt;
    }

    // y axis movement
    if (keyboardState[camera->bindings.jump] && camera->onGround)  // Jump
    {
        camera->yVelocity += JUMPSPEED;
        camera->onGround = 0;
    }
    if (!camera->onGround)
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

void handleCameraRotation(Camera* camera, float motionX, float motionY, double dt)
{
    // fmod is used to prevent yaw from getting too large
    camera->yaw += fmod(motionX * camera->rotationSpeed * dt, 360.0f);
    camera->pitch -= motionY * camera->rotationSpeed * dt;

    // 89 degrees is the limit for pitch
    // This prevents the camera from flipping over
    if (camera->pitch > 89)
        camera->pitch = 89;
    else if (camera->pitch < -89)
        camera->pitch = -89;
}