#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <GL/glu.h>

#include "include/game/camera.h"
#include "include/game/level.h"
#include "include/game/player.h"
#include "include/game/enemy.h"


#define PI 3.14159265358979323846f

#define DEG2RAD(_d) ((_d) * (PI / 180.0f))

#define EYE_Y 1.8f
#define VFOV 70.0f

#define ZNEAR 0.0001f
#define ZFAR  2048.0f

#define DEBUG 1


// Dummy function to draw a cube
// Will be deleted later
float cubeRotationAngle = 0.0f;
void drawCube() {
    glBegin(GL_QUADS);

    // Red side - FRONT
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(-1.0f, -1.0f, 1.0f);
    glVertex3f(1.0f, -1.0f, 1.0f);
    glVertex3f(1.0f, 1.0f, 1.0f);
    glVertex3f(-1.0f, 1.0f, 1.0f);

    // Green side - RIGHT
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(1.0f, -1.0f, 1.0f);
    glVertex3f(1.0f, -1.0f, -1.0f);
    glVertex3f(1.0f, 1.0f, -1.0f);
    glVertex3f(1.0f, 1.0f, 1.0f);

    // Blue face - BACK
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f(-1.0f, -1.0f, -1.0f);
    glVertex3f(1.0f, -1.0f, -1.0f);
    glVertex3f(1.0f, 1.0f, -1.0f);
    glVertex3f(-1.0f, 1.0f, -1.0f);

    // Yellow face - LEFT
    glColor3f(1.0f, 1.0f, 0.0f);
    glVertex3f(-1.0f, -1.0f, -1.0f);
    glVertex3f(-1.0f, 1.0f, -1.0f);
    glVertex3f(-1.0f, 1.0f, 1.0f);
    glVertex3f(-1.0f, -1.0f, 1.0f);

    // Cyan face - TOP
    glColor3f(0.0f, 1.0f, 1.0f);
    glVertex3f(-1.0f, 1.0f, -1.0f);
    glVertex3f(1.0f, 1.0f, -1.0f);
    glVertex3f(1.0f, 1.0f, 1.0f);
    glVertex3f(-1.0f, 1.0f, 1.0f);

    // Magenta face - BOTTOM
    glColor3f(1.0f, 0.0f, 1.0f);
    glVertex3f(-1.0f, -1.0f, -1.0f);
    glVertex3f(1.0f, -1.0f, -1.0f);
    glVertex3f(1.0f, -1.0f, 1.0f);
    glVertex3f(-1.0f, -1.0f, 1.0f);

    glEnd();
}


// Game logic
void update(Camera* camera, Level* level, Player* player, double dt)
{
    return;
}


// Update camera
void updateCamera(Camera* camera)
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(camera->x, camera->y + EYE_Y, camera->z,
              camera->x + sin(DEG2RAD(camera->yaw)) * cos(DEG2RAD(camera->pitch)),
              camera->y + EYE_Y + sin(DEG2RAD(camera->pitch)),
              camera->z - cos(DEG2RAD(camera->yaw)) * cos(DEG2RAD(camera->pitch)),
              0.0f, 1.0f, 0.0f);
}   

void drawLevel(Level* level, Camera* camera)
{
    return;
}

// Render
void render(SDL_Window* window, Camera* camera, Level* level, Player* player)
{
    // Clear screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Update camera
    updateCamera(camera);

    // Draw level
    glPushMatrix();
    drawLevel(level, camera);
    glPopMatrix();

    // Dummy cubes
    // Will be deleted later
    // Rotating cube
    glPushMatrix();
    glTranslatef(0.0f, 0.5f, -3.0f);
    cubeRotationAngle += 0.5f;
    glRotatef(cubeRotationAngle, 0.0f, 1.0f, 0.0f);
    drawCube();
    glPopMatrix();
    // Static cube
    glPushMatrix();
    glTranslatef(0.0f, 0.5f, -7.0f);
    drawCube();
    glPopMatrix();

    // Update screen
    SDL_GL_SwapWindow(window);
}


int main(int argc, char *argv[])
{
    // Initialising SDL
    if (SDL_Init(SDL_INIT_VIDEO))
    {
        fprintf(stderr, "Error initialising SDL : %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    // Getting display information
    uint16_t WINDOW_WIDTH;
    uint16_t WINDOW_HEIGHT;
    if (!DEBUG)
    {
        SDL_DisplayMode displayMode;
        if (SDL_GetCurrentDisplayMode(0, &displayMode))
        {
            fprintf(stderr, "Error getting display information : %s\n", SDL_GetError());
            SDL_Quit();
            return EXIT_FAILURE;
        }
        WINDOW_WIDTH = displayMode.w;
        WINDOW_HEIGHT = displayMode.h;
    }
    else
    {
        WINDOW_WIDTH = 1280;
        WINDOW_HEIGHT = 720;
    }

    // Window creation
    SDL_Window* window = SDL_CreateWindow("Retro FPS", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL);
    if (window == NULL)
    {
        fprintf(stderr, "Error when creating window : %s\n", SDL_GetError());
        SDL_Quit();
        return EXIT_FAILURE;
    }
    SDL_ShowCursor(SDL_DISABLE);
    // Handling fullscreen
    if (!DEBUG)
        SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
    else
        SDL_SetWindowFullscreen(window, 0);

    // OpenGL context creation
    SDL_GLContext glContext = SDL_GL_CreateContext(window);
    if (glContext == NULL)
    {
        fprintf(stderr, "Error when creating OpenGL context : %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return EXIT_FAILURE;
    }
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(VFOV, (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, ZNEAR, ZFAR);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // Loading player
    Player* player = createPlayer();

    // Loading level
    Level* level = loadLevel(player->currentLevel);

    // Loading camera
    Bindings bindings = {
        SDL_SCANCODE_H,  // Forward
        SDL_SCANCODE_B,  // Backward
        SDL_SCANCODE_V,  // Left
        SDL_SCANCODE_N, // Right
        SDL_SCANCODE_G,  // Sprint
        SDL_SCANCODE_SPACE,  // Jump
        SDL_SCANCODE_U,  // Use
        SDL_SCANCODE_I,  // Reload
        SDL_SCANCODE_E,  // Inventory
        SDL_SCANCODE_ESCAPE  // Pause
    };
    Camera* camera = initCamera(level->startX, level->startY, level->startZ, level->startYaw, level->startPitch, player->speed, player->sensitivity, bindings);

    // Loading objects
    // ...

    
    // Main loop
    Uint64 last_frame = SDL_GetTicks64();
    Uint64 now;
    Uint32 dt_ms;
    double dt;
    bool quit = 0;
    SDL_Event e;
    while (!quit)
    {
        // Timing
        now = SDL_GetTicks64();
        dt_ms = now - last_frame;
        // Don't run the game faster than 500 FPS
        if (dt_ms < 3)
        {
            SDL_Delay(3 - dt_ms);
            continue;
        }
        dt = dt_ms / 1000.0;
        last_frame = now;

        // Event loop
        const Uint8* keyboardState = SDL_GetKeyboardState(NULL);
        while (SDL_PollEvent(&e))
        {
            switch (e.type)
            {
                // Quit program
                case SDL_QUIT:
                    quit = 1;
                    break;
                // Rotate camera
                case SDL_MOUSEMOTION:
                    // Power saving
                    if (e.motion.xrel == 0 && e.motion.yrel == 0)
                        break;
                    handleCameraRotation(camera, e.motion.xrel, e.motion.yrel, dt);
                    SDL_WarpMouseInWindow(window, WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
                    break;
                // Shoot
                case SDL_MOUSEBUTTONDOWN:
                    if (e.button.button == SDL_BUTTON_LEFT)
                        printf("Shoot !\n");
                    break;
                default:
                    break;
            }
        }
        // Move camera
        handleCameraMovement(camera, keyboardState, dt);

        // Game logic
        update(camera, level, player, dt);

        // Render
        render(window, camera, level, player);
    }

    // Free memory and quit SDL
    freeCamera(camera);
    camera = NULL;
    freePlayer(player);
    player = NULL;
    freeLevel(level);
    level = NULL;
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    return EXIT_SUCCESS;
}
