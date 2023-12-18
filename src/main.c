#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

int main(int argc, char *argv[])
{
    // Initialisation de SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        fprintf(stderr, "Erreur lors de l'initialisation de SDL : %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    // Création de la fenêtre
    SDL_Window* window = SDL_CreateWindow("SDL Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                          WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL)
    {
        fprintf(stderr, "Erreur lors de la création de la fenêtre SDL : %s\n", SDL_GetError());
        SDL_Quit();
        return EXIT_FAILURE;
    }

    // Création du contexte de rendu
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL)
    {
        fprintf(stderr, "Erreur lors de la création du contexte de rendu : %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return EXIT_FAILURE;
    }

    // Boucle principale du jeu
    bool quit = 0;
    SDL_Event e;
    while (!quit)
    {
        // Vérifie s'il y a eu un événement
        while (SDL_PollEvent(&e) != 0)
        {
            // L'utilisateur demande à quitter le programme
            if (e.type == SDL_QUIT)
                quit = 1;
        }

        // Effacer l'écran
        SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
        SDL_RenderClear(renderer);

        // Mettre à jour et dessiner les éléments du jeu
        // ...

        // Mettre à jour l'affichage
        SDL_RenderPresent(renderer);
    }

    // Libération des ressources et fermeture de SDL
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    return EXIT_SUCCESS;
}
