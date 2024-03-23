#include "camera.h"


// Assuming up is {0.0f, 1.0f, 0.0f}


static void bindingsCopy(Bindings *dst, const Bindings *src)
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

static const Bindings DEFAULT_BINDINGS = {
    SDL_SCANCODE_Z,
    SDL_SCANCODE_S,
    SDL_SCANCODE_Q,
    SDL_SCANCODE_D,
    SDL_SCANCODE_LSHIFT,
    SDL_SCANCODE_SPACE,
    SDL_SCANCODE_E,
    SDL_SCANCODE_R
};

static int importBindings(const char *path, Bindings *dest) {
    FILE *file = fopen(path, "r");
    if (file == NULL) {
        LOG_ERROR("Erreur : Impossible d'ouvrir le fichier %s\n", path);
        return -1;
    }

    char line[100];
    while (fgets(line, sizeof(line), file)) {
        char *key = strtok(line, "=");
        char *value = strtok(NULL, "=");

        if (value != NULL && value[strlen(value) - 1] == '\n') value[strlen(value) - 1] = '\0';

        if (SDL_GetScancodeFromName(value) == SDL_SCANCODE_UNKNOWN) {
            LOG_ERROR("Invalid scancode in %s : %s\n", path, value);
            fclose(file);
            return -1;
        }

        if (strcmp(key, "forward") == 0) {
            dest->forward = SDL_GetScancodeFromName(value);
        } else if (strcmp(key, "backward") == 0) {
            dest->backward = SDL_GetScancodeFromName(value);
        } else if (strcmp(key, "left") == 0) {
            dest->left = SDL_GetScancodeFromName(value);
        } else if (strcmp(key, "right") == 0) {
            dest->right = SDL_GetScancodeFromName(value);
        } else if (strcmp(key, "sprint") == 0) {
            dest->sprint = SDL_GetScancodeFromName(value);
        } else if (strcmp(key, "jump") == 0) {
            dest->jump = SDL_GetScancodeFromName(value);
        } else if (strcmp(key, "use") == 0) {
            dest->use = SDL_GetScancodeFromName(value);
        } else if (strcmp(key, "reload") == 0) {
            dest->reload = SDL_GetScancodeFromName(value);
        }
        else {
            LOG_ERROR("Invalid bindings in %s : %s\n", path, key);
            fclose(file);
            return -1;
        }
    }

    fclose(file);
    return 0;
}

int initCamera(Camera *camera, vec3 pos, vec3 target, const char *bindings)
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
    if (importBindings(bindings, &camera->bindings))
    {
        LOG_ERROR("Could not import custom bindings, using default bindings.\n");
        bindingsCopy(&camera->bindings, &DEFAULT_BINDINGS);
        return -1;
    }
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
    
    // Moving up and down (Debugging purposes)
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