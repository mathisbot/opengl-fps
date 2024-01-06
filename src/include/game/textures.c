#include "textures.h"


int loadTexture(Texture* tex, const char* path, int numMipmaps, bool repeat)
{
    // Loading SDL surface
    char new_path[512];
    sprintf(new_path, "%s%s", TEXTUREPATH, path);
    SDL_Surface* surface = SDL_LoadBMP(new_path);
    if (!surface)
    {
        fprintf(stderr, "Error loading texture %s : %s\n", path, SDL_GetError());
        return -1;
    }

    // Creating OpenGL texture
    GLuint textureID;
    glGenTextures(1, &textureID);

    // Binding texture
    glActiveTexture(GL_TEXTURE0);
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
        static float borderColor[] = { 0.2f, 0.2f, 0.2f, 1.0f };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    }

    glTexStorage2D(GL_TEXTURE_2D, numMipmaps, GL_SRGB8, surface->w, surface->h);
    glTexSubImage2D(
        GL_TEXTURE_2D, 0, 0, 0,
        surface->w, surface->h,
        GL_BGR, GL_UNSIGNED_BYTE, surface->pixels
    );
    glGenerateMipmap(GL_TEXTURE_2D);

    // Setting texture properties
    tex->id = textureID;
    tex->width = surface->w;
    tex->height = surface->h;

    // Freeing SDL surface
    SDL_FreeSurface(surface);

    return 0;
}


void destroyTexture(Texture tex)
{
    glDeleteTextures(1, &tex.id);
}


int createDepthCubemap(GLuint *depthCubemap, unsigned int resolution)
{
    glGenTextures(1, depthCubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, *depthCubemap);
    for (int i=0; i<6; i++)
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X+i, 0, GL_DEPTH_COMPONENT, resolution, resolution, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    return 0;
}

void bindDepthCubemapToFBO(GLuint depthMapFBO, GLuint depthCubemap)
{
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void destroyDepthCubemap(GLuint depthCubemap)
{
    glDeleteTextures(1, &depthCubemap);
}