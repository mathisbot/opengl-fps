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

#include "include/testing/cube.h"


#define DEBUG 1

#define VSYNC 1

#define EYE_Y 1.8f
#define VFOV 70.0f
#define ZNEAR 0.05f
#define ZFAR  32.0f


/*
TODO :
- Rework level loading
    - Encoding?
        - Walls that share a side should be defined with only 2 points
    - Collisions
        - To ensure collisions are fast, sector should be defined
*/


/**
 * @brief Draw a wall
 * 
 * @param wall Wall to draw
 */
void drawWall(Wall* wall) {
    glBindTexture(GL_TEXTURE_2D, wall->textureID);
    glColor4f(1.0, 1.0, 1.0, 1.0);

    if (wall->pointCount == 4 || wall->pointCount == 3)
        glBegin(GL_TRIANGLE_STRIP);
    // Very inefficient, should not be used
    else if (wall->pointCount > 4)
        glBegin(GL_POLYGON);
    // Unknown, skipping
    else
    {
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

    glBindTexture(GL_TEXTURE_2D, 0);
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
        glPushMatrix();
        drawWall(level->walls[i]);
        glPopMatrix();
    }
    glPopMatrix();
}

/**
 * @brief Draw the User Interface
 * 
 * @param w Window width
 * @param h Window height
 * @param pointerType Type of pointer to draw (0 : None, 1 : Crosshair)
 */
void drawUI(int w, int h, int pointerType)
{
    // Draw crosshair
    if (pointerType == 1)
    {
        static float lineThickness = 2.0f;
        static float length = 7.0f;

        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        glOrtho(0, w, h, 0, -1, 1);
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();

        // Crosshair
        glLineWidth(lineThickness);
        glBegin(GL_LINES);
        glColor3f(1.0f, 1.0f, 1.0f);
        glVertex2f(w/2, h/2 - length);
        glVertex2f(w/2, h/2 + length);
        glVertex2f(w/2 - length, h/2);
        glVertex2f(w/2 + length, h/2);
        glEnd();
        // Outline
        glLineWidth(lineThickness+2);
        glBegin(GL_LINES);
        glColor3f(0.0f, 0.0f, 0.0f);
        glVertex2f(w/2, h/2 - length-1);
        glVertex2f(w/2, h/2 + length+1);
        glVertex2f(w/2 - length-1, h/2);
        glVertex2f(w/2 + length+1, h/2);
        glEnd();
        
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();
    }
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
    // Get window size
    static int w=0, h=0;

    if (w == 0 && h == 0)
        SDL_GetWindowSize(SDL_GL_GetCurrentWindow(), &w, &h);

    // Clear screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Update camera
    updateCamera(camera);

    // Draw level
    glPushMatrix();
    drawLevel(level, camera);
    glPopMatrix();

    // Testing cubes
    glPushMatrix();
    drawCube(3.0f, 1.0f, 3.0f, 2.0f);
    glPopMatrix();
    glPushMatrix();
    drawCube(-3.0f, 0.5f, 3.0f, 1.0f);
    glPopMatrix();
    glPushMatrix();
    drawCube(3.0f, 0.5f, -3.0f, 1.0f);
    glPopMatrix();

    // Draw User Interface
    glPushMatrix();
    drawUI(w, h, 1);
    glPopMatrix();

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
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    // SDL_GL_SetAttribute(SDL_GL_STEREO, SDL_ENABLE);

    // OpenGL settings
    if (DEBUG)
    {
        printf("OpenGL version : %s\n", glGetString(GL_VERSION));
        printf("OpenGL vendor : %s\n", glGetString(GL_VENDOR));
        printf("OpenGL renderer : %s\n", glGetString(GL_RENDERER));
        printf("OpenGL shading language version : %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
        printf("Vertex shader version : %s\n", glGetString(GL_VERTEX_SHADER));
        printf("Vertex shader max attribribute count : %d\n", GL_MAX_VERTEX_ATTRIBS);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    }
    glEnable(GL_TEXTURE_2D);
    if (VSYNC)
    {
        // Try to enable Adaptative VSync
        if (SDL_GL_SetSwapInterval(-1) == -1)
        {
            // If it fails, try to enable VSync
            SDL_GL_SetSwapInterval(1);
        }
    }
    // View settings
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(VFOV, (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, ZNEAR, ZFAR);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Optimizations
    // Depth test
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    // Backface culling
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);  // This is the default value
    // Stencil test
    glEnable(GL_STENCIL_TEST);

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
    Bindings* bindings = createBindings(
        SDL_SCANCODE_H,  // Forward
        SDL_SCANCODE_B,  // Backward
        SDL_SCANCODE_V,  // Left
        SDL_SCANCODE_N, // Right
        SDL_SCANCODE_G,  // Sprint
        SDL_SCANCODE_SPACE,  // Jump
        SDL_SCANCODE_U,  // Use
        SDL_SCANCODE_I,  // Reload
        SDL_SCANCODE_E,  // Inventory
        SDL_SCANCODE_K,  // Map
        SDL_SCANCODE_ESCAPE  // Pause
    );
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
        if (DEBUG)
            printf("FPS : %f\n", 1.0 / dt);

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
                    if (e.key.keysym.scancode == camera->bindings->pause)
                    {
                        pause = !pause;
                        if (pause)
                            SDL_ShowCursor(SDL_ENABLE);
                        else
                            SDL_ShowCursor(SDL_DISABLE);
                    }
                    else if (e.key.keysym.scancode == camera->bindings->jump && !pause)
                        cameraJump(camera);
                    else if (e.key.keysym.scancode == SDL_SCANCODE_F1)
                        quit = 1;
                    break;
                // Rotate camera
                case SDL_MOUSEMOTION:
                    // Power saving
                    if ((e.motion.xrel == 0 && e.motion.yrel == 0) || (pause))
                        break;
                    handleCameraRotation(camera, e.motion.xrel, e.motion.yrel, dt);
                    // Ensure mouse stays in the middle of the screen
                    SDL_WarpMouseInWindow(window, WINDOW_WIDTH/2, WINDOW_HEIGHT/2);
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
            if (keyboardState[camera->bindings->sprint])
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
