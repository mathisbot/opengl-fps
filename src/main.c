#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>


#define DEBUG 1

#if DEBUG
    #define WINDOW_WIDTH 1280
    #define WINDOW_HEIGHT 720
#endif


int main(int argc, char *argv[])
{
    // Initialising SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        fprintf(stderr, "Error initialising SDL : %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    // Getting display information
    SDL_DisplayMode displayMode;
    if (SDL_GetCurrentDisplayMode(0, &displayMode) != 0)
    {
        fprintf(stderr, "Error getting display information : %s\n", SDL_GetError());
        SDL_Quit();
        return EXIT_FAILURE;
    }

    #if !DEBUG
        #define WINDOW_WIDTH displayMode.w
        #define WINDOW_HEIGHT displayMode.h
    #endif

    // Window creation
    SDL_Window* window = SDL_CreateWindow("Retro FPS", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL);
    if (window == NULL)
    {
        fprintf(stderr, "Error when creating window : %s\n", SDL_GetError());
        SDL_Quit();
        return EXIT_FAILURE;
    }
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

    // Loading objects
    // ...

    // Main loop
    bool quit = 0;
    SDL_Event e;
    while (!quit)
    {
        // Event loop
        while (SDL_PollEvent(&e) != 0)
        {
            // User requests to close the window
            if (e.type == SDL_QUIT)
                quit = 1;
        }

        // Clear screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Draw here
        // ...

        // Update screen
        SDL_GL_SwapWindow(window);
    }

    // Free memory and quit SDL
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    return EXIT_SUCCESS;
}
