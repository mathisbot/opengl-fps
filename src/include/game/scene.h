#pragma once


#include <cglm/cglm.h>


#include "game/model.h"


typedef struct {
    Model *models;
    unsigned int modelCount;
    bool loaded;

    // Temporary
    vec3 cubePositions[10];
} Scene;


void renderScene(const Scene *scene, GLuint programShader, GLuint VAO);

void destroyScene(Scene *scene);