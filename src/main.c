#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_mixer.h>
#include <GL/glu.h>

#include "include/game/audio.h"
#include "include/game/camera.h"
#include "include/game/level.h"
#include "include/game/models.h"
#include "include/game/player.h"
#include "include/game/enemy.h"
#include "include/game/textures.h"

#include "include/testing/cube.h"


#define DEBUG 1
#define PRINT_FPS 0
#define WIREFRAME 0

#define VSYNC 1

#define EYE_Y 1.8f
#define VFOV 70.0f
#define ZNEAR 0.05f
#define ZFAR  32.0f

#define CROSSHAIR 1
#define NO_POINTER 0


/*
TODO:
- Move to OpenGL Core Profile
    - Create shaders
    - Stop using glBeing/glEnd
    - Stop using glMatrixMode
    - ...
*/


// Game variables that should be free'd
Camera* camera = NULL;
Level* level = NULL;
Player* player = NULL;
// SDL variables that should be free'd
SDL_GLContext glContext = NULL;
SDL_Window* window = NULL;
// Viarables witnessing the initialization of parts that should be free'd
bool mixerInitialized = 0;
bool SDLInitialized = 0;


/**
 * @brief Free memory and quit the program
*/
static void cleanUp()
{
    // Game objects
    if (camera)
    {
        freeCamera(camera);
        camera = NULL;
    }
    if (level)
    {
        freeLevel(level);
        level = NULL;
    }
    if (player)
    {
        freePlayer(player);
        player = NULL;
    }
    // SDL objects
    if (mixerInitialized)
    {
        Mix_CloseAudio();
        mixerInitialized = 0;
    }
    if (glContext)
    {
        SDL_GL_DeleteContext(glContext);
        glContext = NULL;
    }
    if (window)
    {
        SDL_DestroyWindow(window);
        window = NULL;
    }
    if (SDLInitialized)
    {
        SDL_Quit();
        SDLInitialized = 0;
    }
}

/**
 * @brief Free memory and quit the program with an error code and a log
*/
static void cleanUpAndExit(bool exitCode, const char* log, ...)
{
    va_list args;
    va_start(args, log);
    if (log)
        vfprintf(stderr, log, args);
    cleanUp();
    exit(exitCode);
}


/**
 * @brief Update game logic
 * 
 * @param camera Camera of the game
 * @param level Level of the game
 * @param player Player of the game
 * @param dt Time since last frame
 */
void updateGame(Camera* camera, Level* level, Player* player, double dt)
{
    for (int i = 0; i < level->enemyCount; i++)
    {
        updateEnemy(level->enemies[i], dt);
    }
    updatePlayer(player, dt);
}


/**
 * @brief Draw a wall
 * 
 * @param wall Wall to draw
 */
void drawWall(Wall* wall) {
    glBindTexture(GL_TEXTURE_2D, wall->textureID);
    glColor4f(1.0, 1.0, 1.0, 1.0);

    glBegin(GL_TRIANGLE_STRIP);

    // Defining vertices
    // Vertices should be defined in a counter-clockwise order
    for (int i = 0; i < wall->pointCount; i++)
    {
        glTexCoord2f(wall->points[i]->tex_x, wall->points[i]->tex_y);
        glVertex3f(wall->points[i]->x, wall->points[i]->y, wall->points[i]->z);
    }

    glEnd();

    // Unbind texture
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
    for (int i = 0; i < level->wallCount; i++)
    {
        glPushMatrix();
        drawWall(level->walls[i]);
        glPopMatrix();
    }
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
    if (pointerType == CROSSHAIR)
    {
        static float lineThickness = 2.0f;
        static float length = 7.0f;

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
    }

    // Draw weapon
    // TODO
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

    // Window size should not change during the game
    if (w == 0 && h == 0)
        SDL_GetWindowSize(SDL_GL_GetCurrentWindow(), &w, &h);

    // Clear screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Update camera
    updateCamera(camera);

    // Draw level
    drawLevel(level, camera);

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

    // Push and reinitialise matrices
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, w, h, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    drawUI(w, h, CROSSHAIR);

    // Pop matrices
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    // Swap buffers
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
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO))
        cleanUpAndExit(EXIT_FAILURE, "Error initialising SDL : %s", SDL_GetError());
    else
        SDLInitialized = 1;
    if (DEBUG)
        printf("SDL version : %d.%d.%d\n", SDL_MAJOR_VERSION, SDL_MINOR_VERSION, SDL_PATCHLEVEL);

    // Setting OpenGL attributes
    // The code is still made for OpenGL pre-3.3
    // The code needs a huge update to be compatible with OpenGL 3.3+
    // SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    // SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    // SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    if (DEBUG)
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);

    // Getting display information
    uint16_t WINDOW_WIDTH;
    uint16_t WINDOW_HEIGHT;
    // Handling custom resolution
    if (!DEBUG)
    {
        SDL_DisplayMode displayMode;
        if (SDL_GetCurrentDisplayMode(0, &displayMode))
            cleanUpAndExit(EXIT_FAILURE, "Error getting display mode : %s", SDL_GetError());
        WINDOW_WIDTH = displayMode.w;
        WINDOW_HEIGHT = displayMode.h;
    }
    else
    {
        WINDOW_WIDTH = 1280;
        WINDOW_HEIGHT = 720;
    }

    // Window creation
    window = SDL_CreateWindow("Retro FPS", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL);
    if (window == NULL)
        cleanUpAndExit(EXIT_FAILURE, "Error when creating window : %s", SDL_GetError());
    SDL_ShowCursor(SDL_DISABLE);
    // Handling fullscreen
    if (!DEBUG)
        SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
    else
    {
        SDL_SetWindowFullscreen(window, 0);
        // SDL_SetWindowResizable(window, SDL_TRUE);  // Cursed
    }


    // OpenGL context creation
    glContext = SDL_GL_CreateContext(window);
    if (glContext == NULL)
        cleanUpAndExit(EXIT_FAILURE, "Error when creating OpenGL context : %s", SDL_GetError());
    // OpenGL settings
    if (DEBUG)
    {
        printf("> OpenGL version : %s\n", glGetString(GL_VERSION));
        printf("> OpenGL vendor : %s\n", glGetString(GL_VENDOR));
        printf("> OpenGL renderer : %s\n", glGetString(GL_RENDERER));
        printf("> OpenGL shading language version : %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
        printf("> Vertex shader max attribribute count : %d\n", GL_MAX_VERTEX_ATTRIBS);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    }
    if (WIREFRAME && DEBUG)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
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


    // Load audio
    if (Mix_OpenAudio(AUDIO_SAMPLERATE, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, AUDIO_CHUNKSIZE) < 0)
        cleanUpAndExit(EXIT_FAILURE, "Error initialising mixer audio : %s", Mix_GetError());
    else
        mixerInitialized = 1;
    Mix_AllocateChannels(AUDIO_NUMCHANS);


    // Loading player
    player = createPlayer(HP, 1);
    if (!player)
        cleanUpAndExit(EXIT_FAILURE, "Error creating player\n");

    // Loading level
    level = loadLevel(player->currentLevel);
    if (!level)
        cleanUpAndExit(EXIT_FAILURE, "Error loading level\n");

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
    if (!bindings)
        cleanUpAndExit(EXIT_FAILURE, "Error creating bindings for camera\n");
    camera = initCamera(level->startX, level->startY, level->startZ,
                                level->startYaw, level->startPitch, SPEED, SPRINTINGBOOST,
                                SENSITIVITY, bindings, DOUBLEJUMP, AIRCONTROL);
    if (!camera)
    {
        freeBindings(bindings);
        cleanUpAndExit(EXIT_FAILURE, "Error creating camera\n");
    }

    // DUMMY: testing models loading
    // loadModel("shotgun/shotgun");
    // if (DEBUG)
    //     printf("Shotgun loaded\n");


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
        if (DEBUG && PRINT_FPS)
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
        if (!pause)
        {
            if (keyboardState[camera->bindings->sprint])
                handleCameraMovement(camera, keyboardState, dt, camera->sprintingBoost);
            else
                handleCameraMovement(camera, keyboardState, dt, 1.0f);
        }

        // Game logic
        if (!pause)
            updateGame(camera, level, player, dt);

        // Render screen
        render(window, camera, level, player);
    }

    // Clean up memory and exit
    cleanUp();
    
    return EXIT_SUCCESS;
}
