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
#include "include/game/textures.h"


#define EYE_Y 1.8f
#define VFOV 70.0f

#define ZNEAR 0.0001f
#define ZFAR  2048.0f

#define DEBUG 0


/*
TODO :
- Clean up Level structure (abusive use of pointers?)
    - Don't forget to modify level.h, level.c, main.c
- Level rendering needs to be optimized
    - https://en.wikibooks.org/wiki/OpenGL_Programming/Basics/2DObjects#Drawing_a_Series_of_Connected_Shapes_Efficiently
    - Apparently, vertices need to be defined counter-clockwise
*/

/**
 * @brief Draw a wall
 * 
 * @param wall Wall to draw
 */
void drawWall(Wall* wall) {
    glBindTexture(GL_TEXTURE_2D, wall->textureID);

    if (wall->pointCount == 4 || wall->pointCount == 3)
    {
        // GL_TRIANGLE_STRIP allows to render 2 triangles with 4 points (one connected side)
        // Rendering 2 triangles is way more efficient than rendering quads :
        // GPUs only render triangles, CPUs have to break quads into triangles
        glBegin(GL_TRIANGLE_STRIP);
    }
    else if (wall->pointCount > 4)
    {
        // Very inefficient, should not be used
        glBegin(GL_POLYGON);
    }
    else
    {
        // Unknown, skipping
        printf("Unknown pointCount : %d", wall->pointCount);
        return;
    }

    // Defining vertices
    for (int i = 0; i < wall->pointCount; i++)
    {
        glTexCoord2f(wall->points[i]->tex_x, wall->points[i]->tex_y);
        glVertex3f(wall->points[i]->x, wall->points[i]->y, wall->points[i]->z);
    }

    glEnd();
}

/**
 * @brief Draw a level on the screen
 * 
 * @param level Level to draw
 * @param camera Camera to draw from
 */
void drawLevel(Level* level, Camera* camera)
{
    // Draw textured square
    glPushMatrix();
    for (int i = 0; i < level->wallCount; i++)
    {
        drawWall(level->walls[i]);
    }
    glPopMatrix();
}

/**
 * @brief Update game logic
 * 
 * @param camera Camera of the game
 * @param level Level of the game
 * @param player Player of the game
 * @param dt Time since last frame
 */
void update(Camera* camera, Level* level, Player* player, double dt)
{
    return;
}

/**
 * @brief Update the camera on the screen
 * 
 * @param camera Pointer to the camera
 */
void updateCamera(Camera* camera)
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(camera->x, camera->y + EYE_Y, camera->z,
              camera->x + camera->yawSin * camera->pitchCos,
              camera->y + EYE_Y + camera->pitchSin,
              camera->z - camera->yawCos * camera->pitchCos,
              0.0f, 1.0f, 0.0f);
}

/**
 * @brief Render the game
 * 
 * @param window Window of the game
 * @param camera Camera of the game
 * @param level Level of the game
 * @param player Player of the game
 */
void render(SDL_Window* window, Camera* camera, Level* level, Player* player)
{
    // Clear screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // Update camera
    updateCamera(camera);

    // Draw level
    glPushMatrix();
    drawLevel(level, camera);
    glPopMatrix();

    // Draw User Interface
    // ...

    // Flip screen
    SDL_GL_SwapWindow(window);
}


/**
 * @brief Main function
 * 
 * @param argc Number of arguments
 * @param argv Arguments
 * @return int Exit code
 */
int main(int argc, char* argv[])
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
    // Handling custom resolution
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
    Player* player = createPlayer(HP, 1);
    if (!player)
    {
        fprintf(stderr, "Error creating player\n");
        SDL_GL_DeleteContext(glContext);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return EXIT_FAILURE;
    }

    // Loading level
    Level* level = loadLevel(player->currentLevel);
    if (!level)
    {
        fprintf(stderr, "Error loading level\n");
        freePlayer(player);
        player = NULL;
        SDL_GL_DeleteContext(glContext);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return EXIT_FAILURE;
    }

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
    Camera* camera = initCamera(level->startX, level->startY, level->startZ,
                                level->startYaw, level->startPitch, SPEED, SPRINTINGBOOST,
                                SENSITIVITY, bindings, DOUBLEJUMP);
    if (!camera)
    {
        fprintf(stderr, "Error creating camera\n");
        freeLevel(level);
        level = NULL;
        freePlayer(player);
        player = NULL;
        SDL_GL_DeleteContext(glContext);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return EXIT_FAILURE;
    }

    // Main loop
    Uint64 last_frame = SDL_GetTicks64();
    Uint64 now;
    Uint32 dt_ms;
    double dt;
    const Uint8* keyboardState = SDL_GetKeyboardState(NULL);
    bool quit = 0;
    bool pause = 0;
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
        while (SDL_PollEvent(&e))
        {
            switch (e.type)
            {
                // Quit program
                case SDL_QUIT:
                    quit = 1;
                    break;
                // Pause
                case SDL_KEYDOWN:
                    if (e.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
                    {
                        pause = !pause;
                        if (pause)
                            SDL_ShowCursor(SDL_ENABLE);
                        else
                            SDL_ShowCursor(SDL_DISABLE);
                    }
                    else if (e.key.keysym.scancode == SDL_SCANCODE_SPACE && !pause)
                        cameraJump(camera);
                    break;
                // Rotate camera
                case SDL_MOUSEMOTION:
                    // Power saving
                    if ((e.motion.xrel == 0 && e.motion.yrel == 0) || (pause))
                        break;
                    handleCameraRotation(camera, e.motion.xrel, e.motion.yrel, dt);
                    // Ensure mouse stays in the middle of the screen
                    SDL_WarpMouseInWindow(window, WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
                    break;
                // Shoot
                case SDL_MOUSEBUTTONDOWN:
                    if (e.button.button == SDL_BUTTON_LEFT && !pause)
                        // Not implemented yet
                        printf("Shoot !\n");
                    break;
                default:
                    break;
            }
            // Don't bother with the rest of the loop if quitting
            if (quit)
                break;
        }
        // Don't bother with the rest of the loop if quitting
        if (quit)
            break;
        // Move camera
        SDL_PumpEvents();
        if (!pause)
        {
            if (keyboardState[camera->bindings.sprint])
                handleCameraMovement(camera, keyboardState, dt, camera->sprintingBoost);
            else
                handleCameraMovement(camera, keyboardState, dt, 1.0f);
        }

        // Game logic
        if (!pause)
            update(camera, level, player, dt);

        // Render screen
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
