#pragma once


/* --- INCLUDES --- */

#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include <cglm/cglm.h>
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_opengl.h>


#include "game/audio.h"
#include "game/camera.h"
#include "game/light.h"
#include "game/logs.h"
#include "game/model.h"
#include "game/shader.h"
#include "game/textures.h"


/* --- MACROS --- */

// Debug mode
#define DEBUG 1
#define PRINT_FPS 0
#define WIREFRAME 0

// Graphic options
#define VSYNC 0
#define FULLSCREEN 0
#define MSAADEPTH 4

// View options
#define FOV 70.0f
#define ZNEAR 0.1f
#define ZFAR  32.0f


/* --- TYPEDEFS --- */

typedef struct {
    // Context
    SDL_Window* window;
    unsigned int windowWidth, windowHeight;
    SDL_GLContext glContext;
    bool quit;
    bool pause;

    // OpenGL
    GLuint lightVAO;  // Light sources
    GLuint uiVAO;  // UI

    GLuint VBO;  // Vertices
    GLuint uiVBO;  // Vertices for UI

    GLuint depthMapFBO;  // Depth map framebuffer

    GLuint shaderProgram;  // Shader program for scene objects
    GLuint shaderProgramUI;  // Shader program for UI
    GLuint shaderProgramLight;  // Shader program for light
    GLuint shaderProgramDepth;  // Shader program for depth map

    // Properties
    double dt;
    const Uint8 *keyboardState;

    // Game objects
    Camera camera;
    Scene scene;
    PointLight pointLights[4];  // 4 point lights

} Application;


/* --- CLEAN-UP FUNCTIONS --- */


/**
 * @brief Run application clean-up
 * 
 * @param app Pointer to application
*/
int appRun(Application* app);
