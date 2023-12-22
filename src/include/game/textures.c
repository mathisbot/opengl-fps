#include "textures.h"


// Load textures
Texture* loadTexture(const char* path, bool repeat)
{
    // Allocating memory for texture
    Texture* texture = malloc(sizeof(Texture));
    if (!texture)
    {
        fprintf(stderr, "Error allocating memory for texture.\n");
        exit(1);
    }

    // Loading SDL surface
    char new_path[512];
    sprintf(new_path, "%s%s", TEXTUREPATH, path);
    SDL_Surface* surface = SDL_LoadBMP(new_path);
    if (!surface)
    {
        fprintf(stderr, "Error loading texture %s : %s\n", path, SDL_GetError());
        exit(1);
    }

    // Creating OpenGL texture
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGB,
        surface->w, surface->h, 0,
        GL_BGR, GL_UNSIGNED_BYTE, surface->pixels
    );

    // Freeing SDL surface
    SDL_FreeSurface(surface);

    // Setting texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // Repeat texture
    if (repeat)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }

    // Setting texture properties
    texture->id = textureID;
    strcpy(texture->path, path);
    texture->width = surface->w;
    texture->height = surface->h;
    
    // Returning textureID
    return texture;
}

// Free texture
void freeTexture(Texture* texture)
{
    glDeleteTextures(1, &texture->id);
    free(texture);
}
