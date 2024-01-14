/*
 * (c) [2023-2024] [Dozer35]
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * 
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 * 
 * Description: The aim of this project is to create a simple FPS game,
 * using no game engine. This allows for a low-level approach to game
 * making, and therefore very thorough optimization.
*/

/* --- TODO --- */

/*
- Stencil test for UI ?
- Find other ways to load sounds (WAV is too big)
    -> OGG ?
- SDL_image for other formats than BMP ?
*/


/* --- INCLUDES --- */

#include "include/core/app.h"


/* --- MAIN --- */

/**
 * @brief Main function
 * 
 * @param argc Number of arguments
 * @param argv Arguments
 * @return int Exit code
*/
int main(int argc, char *argv[])
{
    // These arguments are unused for now
    (void)argc;
    (void)argv;


    /* --- Initialise Components --- */

    Application app = {0};
    appRun(&app);

    return EXIT_SUCCESS;
}
