#include "textures.h"


Texture* loadTexture(const char* path, bool repeat)
{
    // Allocating memory for texture
    Texture* texture = malloc(sizeof(Texture));
    if (!texture)
    {
        fprintf(stderr, "Error allocating memory for texture.\n");
        return NULL;
    }

    // Loading SDL surface
    char new_path[512];
    sprintf(new_path, "%s%s", TEXTUREPATH, path);
    SDL_Surface* surface = SDL_LoadBMP(new_path);
    if (!surface)
    {
        fprintf(stderr, "Error loading texture %s : %s\n", path, SDL_GetError());
        return NULL;
    }

    // Creating OpenGL texture
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Setting texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // Repeat texture
    if (repeat)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }
    else
    {
        // Clamp texture
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        static float borderColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    }
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGB,
        surface->w, surface->h, 0,
        GL_BGR, GL_UNSIGNED_BYTE, surface->pixels
    );

    // Freeing SDL surface
    SDL_FreeSurface(surface);

    // Setting texture properties
    texture->id = textureID;
    strcpy(texture->path, path);
    texture->width = surface->w;
    texture->height = surface->h;
    
    // Returning textureID
    return texture;
}

void freeTexture(Texture* texture)
{
    glDeleteTextures(1, &texture->id);
    free(texture);
}
