#include "scene.h"


void renderScene(const Scene *scene, GLuint programShader)
{
    glUseProgram(programShader);

    for (unsigned int i=0; i<scene->modelCount; i++) drawModel(&scene->models[i], programShader);
}

void destroyScene(Scene *scene)
{
    for (unsigned int i=0; i<scene->modelCount; i++)
        freeModel(&scene->models[i]);
    destroySkybox(scene);
    free(scene->models);
}


int loadSkybox(Scene* scene, char *folder)
{
    return loadCubemap(&scene->skybox, folder, "bmp");
}

void destroySkybox(Scene* scene)
{
    destroyCubemap(&scene->skybox);
}