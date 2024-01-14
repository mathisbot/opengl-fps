#pragma once


#include <cglm/cglm.h>


#include "game/model.h"
#include "game/textures.h"


typedef struct {
    Model *models;
    unsigned int modelCount;

    Cubemap skybox;

    bool loaded;
} Scene;


void renderScene(const Scene *scene, GLuint programShader);

/**
 * @brief Load a scene
 * 
 * @param scene Pointer to the scene
 * @param folder Folder containing the skybox
 * @return int 0 if success, -1 if error
*/
int loadSkybox(Scene* scene, char *folder);

void destroySkybox(Scene* scene);

void destroyScene(Scene *scene);