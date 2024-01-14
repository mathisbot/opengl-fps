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
    free(scene->models);
}