#include "textures.h"


int loadTexture(Texture *tex, const char* path, int numMipmaps, bool repeat, uint8_t type)
{
    char new_path[512];
    sprintf(new_path, "%s%s", TEXTUREPATH, path);
    return loadTextureFullPath(tex, new_path, numMipmaps, repeat, type);
}

int loadTextureFullPath(Texture *tex, const char* path, int numMipmaps, bool repeat, uint8_t type)
{
    // Loading SDL surface
    SDL_Surface* surface = SDL_LoadBMP(path);
    if (!surface)
    {
        LOG_ERROR("Error loading texture %s : %s\n", path, SDL_GetError());
        return -1;
    }

    // Creating OpenGL texture
    GLuint textureID;
    glGenTextures(1, &textureID);

    // Binding texture properly
    static int lastBoundTexture = -1;
    glActiveTexture(GL_TEXTURE0);
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &lastBoundTexture);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Setting texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
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
        const float borderColor[] = { 0.2f, 0.2f, 0.2f, 1.0f };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    }


    glTexStorage2D(GL_TEXTURE_2D, numMipmaps, GL_SRGB8, surface->w, surface->h);
    if (type==TEXTURE_DIFFUSE) glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, surface->w, surface->h, GL_BGR, GL_UNSIGNED_BYTE, surface->pixels);
    if (type==TEXTURE_SPECULAR) glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, surface->w, surface->h, GL_RED, GL_UNSIGNED_BYTE, surface->pixels);
    if (type==TEXTURE_NORMAL) glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, surface->w, surface->h, GL_BGR, GL_UNSIGNED_BYTE, surface->pixels);
    if (type==TEXTURE_HEIGHT) glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, surface->w, surface->h, GL_RED, GL_UNSIGNED_BYTE, surface->pixels);
    glGenerateMipmap(GL_TEXTURE_2D);

    // Unbinding texture
    glBindTexture(GL_TEXTURE_2D, lastBoundTexture);

    // Setting texture properties
    tex->id = textureID;
    tex->width = surface->w;
    tex->height = surface->h;
    tex->type = type;
    strncpy(tex->path, path, 256);

    // Freeing SDL surface
    SDL_FreeSurface(surface);

    LOG_DEBUG("Loaded texture %s\n", path);

    return 0;
}


void destroyTexture(Texture tex)
{
    glDeleteTextures(1, &tex.id);
}
