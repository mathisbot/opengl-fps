#include "camera.h"


// Assuming up is {0.0f, 1.0f, 0.0f}


static void bindingsCopy(Bindings *dst, Bindings *src)
{
    dst->forward = src->forward;
    dst->backward = src->backward;
    dst->left = src->left;
    dst->right = src->right;
    dst->sprint = src->sprint;
    dst->jump = src->jump;
    dst->use = src->use;
    dst->reload = src->reload;
}

int initCamera(Camera *camera, vec3 pos, vec3 target, Bindings *bindings)
{
    glm_vec3_copy(pos, camera->pos);
    glm_vec3_copy(target, camera->target);
    glm_vec3_copy((vec3){0.0f, 1.0f, 0.0f}, camera->up);
    glm_vec3_normalize(camera->up);
    glm_vec3_sub(camera->pos, camera->target, camera->direction);
    glm_vec3_normalize(camera->direction);
    glm_vec3_cross(camera->up, camera->direction, camera->right);
    glm_vec3_normalize(camera->right);
    camera->speed = SPEED;
    camera->sprintBoost = SPRINTBOOST;
    camera->sensitivity = SENSITIVITY;
    camera->onGround = 1;
    camera->jumpSpeed = JUMPSPEED;
    glm_vec3_copy(GLM_VEC3_ZERO, camera->upVelocity);
    bindingsCopy(&camera->bindings, bindings);
    return 0;
}


void translateCamera(Camera *camera, const Uint8* keyboardState, double dt)
{
    vec3 translation = {0.0f, 0.0f, 0.0f};

    // (Direction2D, Right2D) plane movement

    if (keyboardState[camera->bindings.forward])
    {
        glm_vec3_sub(translation, camera->direction2D, translation);
        if (keyboardState[camera->bindings.sprint])
            glm_vec3_scale(translation, camera->sprintBoost, translation);
    }
    if (keyboardState[camera->bindings.backward])
        glm_vec3_add(translation, camera->direction2D, translation);
    if (keyboardState[camera->bindings.left])
        glm_vec3_sub(translation, camera->right2D, translation);
    if (keyboardState[camera->bindings.right])
        glm_vec3_add(translation, camera->right2D, translation);
    glm_vec3_scale(translation, camera->speed * dt, translation);

    // Up vector (gravity and jumping)
    
    // Moving up and down
    // vec3 upmov;
    // glm_vec3_scale(camera->up, 5.0f*dt, upmov);
    // if (keyboardState[SDL_SCANCODE_DOWN])
    //     glm_vec3_sub(translation, upmov, translation);
    // if (keyboardState[SDL_SCANCODE_UP])
    //     glm_vec3_add(translation, upmov, translation);

    // Jumping
    if (keyboardState[camera->bindings.jump] && camera->onGround)
    {
        vec3 jump;
        glm_vec3_scale(camera->up, camera->jumpSpeed, jump);
        glm_vec3_copy(jump, camera->upVelocity);
        camera->onGround = 0;
    }
    // Falling
    else if (!camera->onGround)
    {
        vec3 gravity;
        vec3 yTranslation;

        // Gravity
        glm_vec3_scale(camera->up, -GRAVITY * dt, gravity);
        glm_vec3_add(camera->upVelocity, gravity, camera->upVelocity);
        glm_vec3_scale(camera->upVelocity, dt, yTranslation);

        // Check if camera is below ground
        // Currently assuming that the ground is at y=0
        if (camera->pos[1]+yTranslation[1] < EYE_Y)
        {
            // Move camera to ground
            camera->upVelocity[1] = -camera->pos[1] + EYE_Y;
            glm_vec3_add(translation, camera->upVelocity, translation);

            // Reset velocity
            camera->onGround = 1;
            glm_vec3_copy(GLM_VEC3_ZERO, camera->upVelocity);
        }
        else
        {
            glm_vec3_add(translation, yTranslation, translation);
        }
    }

    // Update position
    glm_vec3_add(camera->pos, translation, camera->pos);
    glm_vec3_add(camera->target, translation, camera->target);
}


void rotateCamera(Camera *camera, int dx, int dy)
{
    static float yaw = 0.0f;
    static float pitch = 0;
    vec3 addTarget = {0.0f, 0.0f, 0.0f};

    // fmod is used to prevent yaw from getting too large
    // thus keeping precision in the float
    yaw += fmod(dx * camera->sensitivity, 360.0f);
    pitch -= dy * camera->sensitivity;
    // This prevents the camera from flipping over
    pitch = glm_clamp(pitch, -89.0f, 89.0f);

    // Update target
    addTarget[0] = cosf(glm_rad(yaw)) * cosf(glm_rad(pitch));
    addTarget[1] = sinf(glm_rad(pitch));
    addTarget[2] = sinf(glm_rad(yaw)) * cosf(glm_rad(pitch));
    glm_vec3_add(addTarget, camera->pos, camera->target);
}


void updateCamera(Camera *camera)
{
    // Update direction and right
    glm_vec3_sub(camera->pos, camera->target, camera->direction);
    glm_vec3_normalize(camera->direction);
    glm_vec3_cross(camera->up, camera->direction, camera->right);
    glm_vec3_copy(camera->direction, camera->direction2D);
    camera->direction2D[1] = 0.0f;
    glm_vec3_normalize(camera->direction2D);
    glm_vec3_cross(camera->up, camera->direction2D, camera->right2D);
}