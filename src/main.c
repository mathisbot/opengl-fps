#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>


#define DEBUG 0

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
                                          WINDOW_WIDTH, WINDOW_HEIGHT, 0);
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

    // Renderer creation
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL)
    {
        fprintf(stderr, "Error when creating renderer : %s\n", SDL_GetError());
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
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);

        // Draw here
        // ...

        // Update screen
        SDL_RenderPresent(renderer);
    }

    // Free memory and quit SDL
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    return EXIT_SUCCESS;
}
