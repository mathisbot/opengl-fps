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
- Better textures
    -> https://learnopengl.com/Advanced-Lighting/Normal-Mapping
    -> https://learnopengl.com/Advanced-Lighting/Parallax-Mapping
    -> https://learnopengl.com/Advanced-Lighting/HDR
    -> https://learnopengl.com/Advanced-Lighting/Bloom
- Find other ways to load sounds (WAV is too big)
    -> OGG ?
*/


/* --- INCLUDES --- */

#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>  // For performance measurement

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_mixer.h>

#include <cglm/cglm.h>

#include "include/game/audio.h"
#include "include/game/camera.h"
#include "include/game/collision.h"
#include "include/game/light.h"
#include "include/game/shader.h"
#include "include/game/textures.h"


/* --- MACROS --- */

// Debug mode
#define DEBUG 1
#define PRINT_FPS 0
#define WIREFRAME 0

// Graphic options
#define VSYNC 1
#define FULLSCREEN 0
#define MSAADEPTH 4

// View options
#define FOV 70.0f
#define ZNEAR 0.1f
#define ZFAR  32.0f


/* --- GLOBAL VARIABLES --- */


// Game
Camera camera;
PointLight pointLights[4];  // 4 point lights
// Level level;
// ...

// SDL
static SDL_Window* window = NULL;
static SDL_GLContext glContext = NULL;

// OpenGL
static GLuint lightVAO = 0;  // Light sources
static GLuint uiVAO = 0;  // UI
static GLuint VAO = 0;  // Scene Objects

static GLuint VBO = 0;  // Vertices
static GLuint uiVBO = 0;  // Vertices for UI
static GLuint EBO = 0;  // Indices

static GLuint depthMapFBO = 0;  // Depth map framebuffer

static Shader vertexShader = {0};  // Vertex shader
static Shader vertexShaderUI = {0};  // Vertex shader for UI
static Shader vertexShaderDepth = {0};  // Vertex shader for depth map
static Shader geometryShaderDepth = {0};  // Geometry shader for depth map
static Shader fragmentShader = {0};  // Fragment shader for scene objects
static Shader fragmentShaderUI = {0};  // Fragment shader for UI
static Shader fragmentShaderLight = {0};  // Fragment shader for light
static Shader fragmentShaderDepth = {0};  // Fragment shader for depth map

static GLuint shaderProgram = 0;  // Shader program for scene objects
static GLuint shaderProgramUI = 0;  // Shader program for UI
static GLuint shaderProgramLight = 0;  // Shader program for light
static GLuint shaderProgramDepth = 0;  // Shader program for depth map


/* --- CLEAN-UP FUNCTIONS --- */

static bool SDLInitialized = 0;
static bool mixerInitalized = 0;

/**
 * @brief Free memory
*/
static void cleanUp()
{
    // SDL
    if (glContext) {SDL_GL_DeleteContext(glContext); glContext = NULL;}
    if (window) {SDL_DestroyWindow(window); window = NULL;}
    if (SDLInitialized) {SDL_Quit(); SDLInitialized = 0;}
    if (mixerInitalized) {Mix_CloseAudio(); Mix_Quit(); mixerInitalized = 0;}

    // OpenGL
    if (VBO) {glDeleteBuffers(1, &VBO); VBO = 0;}
    if (uiVBO) {glDeleteBuffers(1, &uiVBO); uiVBO = 0;}
    if (EBO) {glDeleteBuffers(1, &EBO); EBO = 0;}
    if (VAO) {glDeleteVertexArrays(1, &VAO); VAO = 0;}

    if (depthMapFBO) {glDeleteFramebuffers(1, &depthMapFBO); depthMapFBO = 0;}

    if (lightVAO) {glDeleteVertexArrays(1, &lightVAO); lightVAO = 0;}
    if (uiVAO) {glDeleteVertexArrays(1, &uiVAO); uiVAO = 0;}

    if (vertexShader.id) {destroyShader(&vertexShader); memset(&vertexShader, 0, sizeof(Shader));}
    if (vertexShaderUI.id) {destroyShader(&vertexShaderUI); memset(&vertexShaderUI, 0, sizeof(Shader));}
    if (vertexShaderDepth.id) {destroyShader(&vertexShaderDepth); memset(&vertexShaderDepth, 0, sizeof(Shader));}
    if (geometryShaderDepth.id) {destroyShader(&geometryShaderDepth); memset(&geometryShaderDepth, 0, sizeof(Shader));}
    if (fragmentShader.id) {destroyShader(&fragmentShader); memset(&fragmentShader, 0, sizeof(Shader));}
    if (fragmentShaderUI.id) {destroyShader(&fragmentShaderUI); memset(&fragmentShaderUI, 0, sizeof(Shader));}
    if (fragmentShaderLight.id) {destroyShader(&fragmentShaderLight); memset(&fragmentShaderLight, 0, sizeof(Shader));}
    if (fragmentShaderDepth.id) {destroyShader(&fragmentShaderDepth); memset(&fragmentShaderDepth, 0, sizeof(Shader));}

    if (shaderProgram) {glDeleteProgram(shaderProgram); shaderProgram = 0;}
    if (shaderProgramUI) {glDeleteProgram(shaderProgramUI); shaderProgramUI = 0;}
    if (shaderProgramLight) {glDeleteProgram(shaderProgramLight); shaderProgramLight = 0;}
    if (shaderProgramDepth) {glDeleteProgram(shaderProgramDepth); shaderProgramDepth = 0;}

    // Freeing other components
    for (uint8_t i=0; i<sizeof(pointLights)/sizeof(PointLight); i++) destroyPointLight(&pointLights[i]);
}

/**
 * @brief Free memory and quit the program with an error code and a log
 * 
 * @param exitCode Error code
 * @param log Log to print
 * @param ... Arguments for the log
*/
static void cleanUpAndExit(bool exitCode, const char* log, ...)
{
    va_list args;
    va_start(args, log);
    if (log)
        vfprintf(stderr, log, args);
    va_end(args);
    cleanUp();
    exit(exitCode);
}


/* --- GL ERRORS --- */
// Used only if debug mode is enabled
#if DEBUG

void debugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam)
{
    if(id == 131169 || id == 131185 || id == 131218 || id == 131204)
        return;
    fprintf(stderr, "---------------\n");
    fprintf(stderr, "Debug message (%d): %s\n", id, message);
    switch (source)
    {
        case GL_DEBUG_SOURCE_API:
            fprintf(stderr, "Source: API");
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
            fprintf(stderr, "Source: Window System"); break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER:
            fprintf(stderr, "Source: Shader Compiler"); break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:
            fprintf(stderr, "Source: Third Party"); break;
        case GL_DEBUG_SOURCE_APPLICATION:
            fprintf(stderr, "Source: Application"); break;
        case GL_DEBUG_SOURCE_OTHER:
            fprintf(stderr, "Source: Other"); break;
    }
    fprintf(stderr, "\n");
    switch (type)
    {
        case GL_DEBUG_TYPE_ERROR:
        fprintf(stderr, "Type: Error"); break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        fprintf(stderr, "Type: Deprecated Behaviour"); break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        fprintf(stderr, "Type: Undefined Behaviour"); break;
        case GL_DEBUG_TYPE_PORTABILITY:
        fprintf(stderr, "Type: Portability"); break;
        case GL_DEBUG_TYPE_PERFORMANCE:
        fprintf(stderr, "Type: Performance"); break;
        case GL_DEBUG_TYPE_MARKER:
       fprintf(stderr, "Type: Marker"); break;
        case GL_DEBUG_TYPE_PUSH_GROUP:
        fprintf(stderr, "Type: Push Group"); break;
        case GL_DEBUG_TYPE_POP_GROUP:
        fprintf(stderr, "Type: Pop Group"); break;
        case GL_DEBUG_TYPE_OTHER:
        fprintf(stderr, "Type: Other"); break;
    }
    fprintf(stderr, "\n");
    switch (severity)
    {
        case GL_DEBUG_SEVERITY_HIGH:
        fprintf(stderr, "Severity: high"); break;
        case GL_DEBUG_SEVERITY_MEDIUM:
        fprintf(stderr, "Severity: medium"); break;
        case GL_DEBUG_SEVERITY_LOW:
        fprintf(stderr, "Severity: low"); break;
        case GL_DEBUG_SEVERITY_NOTIFICATION:
        fprintf(stderr, "Severity: notification"); break;
    }
    fprintf(stderr, "\n\n");
}

#endif


/* --- GAME LOGIC --- */

/**
 * @brief Update the game
*/
void updateGame()
{
    // TODO
    return;
}


/* --- RENDERING FUNCTION --- */

/**
 * @brief Render the screen
 * 
 * @param width Width of the screen
 * @param height Height of the screen
*/
static void render(GLuint width, GLuint height)
{
    // Cubes positions
    vec3 cubePositions[10];
    float positions[10][3] = {
        { 0.0f,  0.0f,  0.0f},
        { 2.0f,  5.0f, -9.0f},
        { 2.4f,  0.4f, -3.5f},
        {-1.7f,  3.0f, -7.5f},
        { 1.5f,  2.0f, -2.5f},
        { 1.5f,  0.2f, -1.5f},
        {-1.3f,  1.0f, -1.5f},
        { 0.0f,  1.0f,  5.0f},
        {-0.5f,  2.0f,  7.5f},
        { 1.0f,  0.5f,  7.5f},
    };
    // Ugliest way
    for (int i = 0; i < 10; ++i) {
        for (int j = 0; j < 3; ++j) {
            cubePositions[i][j] = positions[i][j];
        }
    }


    /* --- RENDER ON DEPTH MAP --- */

    glViewport(0, 0, SHADOWMAP_RES, SHADOWMAP_RES);
    glUseProgram(shaderProgramDepth);
    glBindVertexArray(VAO);

    glUniform1f(glGetUniformLocation(shaderProgramDepth, "farPlane"), SHADOWMAP_ZFAR);

    // Shaders and matrices
    static mat4 lightProjection;
    glm_perspective(glm_rad(90.0f), 1.0f, SHADOWMAP_ZNEAR, SHADOWMAP_ZFAR, lightProjection);

    // Compute depth map for each light
    static mat4 shadowMatrices[6];
    for (uint8_t i=0; i<4; i++)
    {
        // Each light has its own depth cubemap
        bindPointLightToFBO(depthMapFBO, &(pointLights[i]));

        glUniform3f(glGetUniformLocation(shaderProgramDepth, "lightPos"), pointLights[i].position[0], pointLights[i].position[1], pointLights[i].position[2]);

        pointLightGetProjMatrices(&(pointLights[i]), &lightProjection, &shadowMatrices);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgramDepth, "shadowMatrices"), 6, GL_FALSE, (float*)(shadowMatrices));

        // Rendering
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);

        for (uint8_t i=0; i<10; i++)
        {
            // Model matrix
            static mat4 model = GLM_MAT4_IDENTITY_INIT;
            glm_mat4_identity(model);

            glm_translate(model, cubePositions[i]);
            if (i==0) glm_scale(model, (vec3){30.0f, 0.01f, 30.0f});
            glm_rotate(model, glm_rad(20.0f*i), (vec3){0.5f, 1.0f, 0.0f});

            glUniformMatrix4fv(glGetUniformLocation(shaderProgramDepth, "model"), 1, GL_FALSE, (float*)model);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
    }


    /* --- RENDER ON SCREEN --- */

    glViewport(0, 0, width, height);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    // Order : UI, Light, Objects


    /* --- UI --- */

    glUseProgram(shaderProgramUI);

    glUniform2ui(glGetUniformLocation(shaderProgramUI, "windowSize"), width, height);
    glUniform1f(glGetUniformLocation(shaderProgramUI, "pointerRadius"), 2.0f);

    glBindVertexArray(uiVAO);

    // Simple overlay
    glDrawArrays(GL_TRIANGLES, 0, 6);


    /* --- Light sources --- */

    // Use light shader
    glUseProgram(shaderProgramLight);

    // View matrix
    static mat4 view = GLM_MAT4_IDENTITY_INIT;
    glm_lookat(camera.pos, camera.target, camera.up, view);

    // Projection matrix
    static mat4 projection = GLM_MAT4_IDENTITY_INIT;
    glm_mat4_identity(projection);
    glm_perspective(glm_rad(FOV), (float)width / (float)height, ZNEAR, ZFAR, projection);

    // Send to shader
    glUniformMatrix4fv(glGetUniformLocation(shaderProgramLight, "view"), 1, GL_FALSE, (float*)view);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgramLight, "projection"), 1, GL_FALSE, (float*)projection);
    
    // Used to draw pointer
    glUniform2ui(glGetUniformLocation(shaderProgramLight, "windowSize"), width, height);
    glUniform1f(glGetUniformLocation(shaderProgramLight, "pointerRadius"), 2.0f);

    // Rendering
    glBindVertexArray(lightVAO);
    
    for (uint8_t i=0; i<4; i++)
    {
        glUniform3f(glGetUniformLocation(shaderProgramLight, "lightColor"), pointLights[i].color[0], pointLights[i].color[1], pointLights[i].color[2]);

        // Model matrix
        static mat4 modelLight = GLM_MAT4_IDENTITY_INIT;
        glm_mat4_identity(modelLight);
        glm_translate(modelLight, pointLights[i].position);
        glm_scale(modelLight, (vec3){0.2f, 0.2f, 0.2f});

        glUniformMatrix4fv(glGetUniformLocation(shaderProgramLight, "model"), 1, GL_FALSE, (float*)modelLight);

        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    glBindVertexArray(0);


    /* --- Objects --- */

    glUseProgram(shaderProgram);

    // Send to shader
    // Matrices are unchanged from light rendering
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, (float*)view);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, (float*)projection);

    glUniform3f(glGetUniformLocation(shaderProgram, "viewPos"), camera.pos[0], camera.pos[1], camera.pos[2]);
    glUniform1f(glGetUniformLocation(shaderProgram, "farPlane"), SHADOWMAP_ZFAR);

    // Will be useful later to avoid binding too many textures
    static int textureUnits = 0;
    if (!textureUnits) glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &textureUnits);
    static const int textureStart = 1;  // 0 is reserved for face texture

    char locate[32];
    for (uint8_t i=0; i<4; i++)
    {
        sprintf(locate, "pointLights[%d].position", i);
        glUniform3f(glGetUniformLocation(shaderProgram, locate), pointLights[i].position[0], pointLights[i].position[1], pointLights[i].position[2]);
        sprintf(locate, "pointLights[%d].ambiant", i);
        glUniform3f(glGetUniformLocation(shaderProgram, locate), 0.2f, 0.2f, 0.2f);
        sprintf(locate, "pointLights[%d].diffuse", i);
        glUniform3f(glGetUniformLocation(shaderProgram, locate), 0.5f, 0.5f, 0.5f);
        sprintf(locate, "pointLights[%d].specular", i);
        glUniform3f(glGetUniformLocation(shaderProgram, locate), 1.0f, 1.0f, 1.0f);
        sprintf(locate, "pointLights[%d].linear", i);
        glUniform1f(glGetUniformLocation(shaderProgram, locate), 0.01f);
        sprintf(locate, "pointLights[%d].quadratic", i);
        glUniform1f(glGetUniformLocation(shaderProgram, locate), 0.02f);
        sprintf(locate, "pointLights[%d].color", i);
        glUniform3f(glGetUniformLocation(shaderProgram, locate), pointLights[i].color[0], pointLights[i].color[1], pointLights[i].color[2]);
        sprintf(locate, "pointLights[%d].depthCubemap", i);
        glActiveTexture(GL_TEXTURE0+i+textureStart);
        glBindTexture(GL_TEXTURE_CUBE_MAP, pointLights[i].depthCubemap);
        glUniform1i(glGetUniformLocation(shaderProgram, locate), i+textureStart);
    }

    glUniform2ui(glGetUniformLocation(shaderProgram, "windowSize"), width, height);
    glUniform1f(glGetUniformLocation(shaderProgram, "pointerRadius"), 2.0f);

    glUniform3f(glGetUniformLocation(shaderProgram, "material.ambient"), 1.0f, 0.5f, 0.31f);
    glUniform3f(glGetUniformLocation(shaderProgram, "material.diffuse"), 1.0f, 0.5f, 0.31f);
    glUniform3f(glGetUniformLocation(shaderProgram, "material.specular"), 0.5f, 0.5f, 0.5f);
    glUniform1f(glGetUniformLocation(shaderProgram, "material.shininess"), 64.0f);

    // Rendering
    glBindVertexArray(VAO);
    for (uint8_t i=0; i<10; i++)
    {
        // Model matrix
        static mat4 model = GLM_MAT4_IDENTITY_INIT;
        glm_mat4_identity(model);

        glm_translate(model, cubePositions[i]);
        if (i==0) glm_scale(model, (vec3){30.0f, 0.01f, 30.0f});
        glm_rotate(model, glm_rad(20.0f*i), (vec3){0.5f, 1.0f, 0.0f});

        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, (float*)model);

        // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }


    // Unbind everything
    glBindVertexArray(0);
    glUseProgram(0);


    // Swap buffers
    SDL_GL_SwapWindow(window);
}


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

    // Initialising SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO))
        cleanUpAndExit(EXIT_FAILURE, "Error initialising SDL : %s", SDL_GetError());
    else SDLInitialized = 1;
    if (DEBUG) printf("SDL version : %d.%d.%d\n", SDL_MAJOR_VERSION, SDL_MINOR_VERSION, SDL_PATCHLEVEL);

    // Getting display information for window creation
    uint16_t WINDOW_WIDTH;
    uint16_t WINDOW_HEIGHT;
    // Handling custom resolution
    if (FULLSCREEN)
    {
        SDL_DisplayMode displayMode;
        if (SDL_GetCurrentDisplayMode(0, &displayMode))
            cleanUpAndExit(EXIT_FAILURE, "Error getting display mode : %s", SDL_GetError());
        WINDOW_WIDTH = displayMode.w;
        WINDOW_HEIGHT = displayMode.h;
    }
    else {WINDOW_WIDTH = 1280; WINDOW_HEIGHT = 720;}

    // OpenGL attributes
    // OpenGL version 4.6.0 Core profile
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);

    if (DEBUG) SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, MSAADEPTH);


    // Window creation
    window = SDL_CreateWindow("Retro FPS", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL);
    if (window == NULL) cleanUpAndExit(EXIT_FAILURE, "Error when creating window : %s", SDL_GetError());
    SDL_ShowCursor(SDL_DISABLE);
    // Handling fullscreen
    SDL_SetWindowFullscreen(window, FULLSCREEN ? SDL_WINDOW_FULLSCREEN : 0);


    // OpenGL context creation
    glContext = SDL_GL_CreateContext(window);
    if (glContext == NULL) cleanUpAndExit(EXIT_FAILURE, "Error when creating OpenGL context : %s", SDL_GetError());


    // GLEW initialization
    GLenum glewInitState = glewInit();
    if (glewInitState != GLEW_OK) cleanUpAndExit(EXIT_FAILURE, "Error initialising GLEW : %s", glewGetErrorString(glewInitState));


    // OpenGL settings
    #if DEBUG
    GLint flags; glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
    {
        printf("[GL Debug Output enabled]\n");
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(debugCallback, NULL);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
    
    }
    else fprintf(stderr, "[GL Debug Output failed to activate]");
    printf("> OpenGL version : %s\n", glGetString(GL_VERSION));
    printf("> OpenGL vendor : %s\n", glGetString(GL_VENDOR));
    printf("> OpenGL renderer : %s\n", glGetString(GL_RENDERER));
    printf("> OpenGL shading language version : %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
    printf("> Vertex shader max attribribute count : %d\n", GL_MAX_VERTEX_ATTRIBS);
    #endif

    if (VSYNC) {if (SDL_GL_SetSwapInterval(-1) == -1) SDL_GL_SetSwapInterval(1);}
    if (WIREFRAME) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_CULL_FACE);  // Triangles have to be defined in counter-clockwise order
    glEnable(GL_FRAMEBUFFER_SRGB);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);


    // OpenGL Buffer creation
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &uiVBO);
    glGenBuffers(1, &EBO);

    glGenFramebuffers(1, &depthMapFBO);
    
    glGenVertexArrays(1, &VAO);
    glGenVertexArrays(1, &lightVAO);
    glGenVertexArrays(1, &uiVAO);

    // OpenGL Shader creation
    if (loadShader(&vertexShader, "vertex.vert", GL_VERTEX_SHADER) < 0) cleanUpAndExit(EXIT_FAILURE, "Error creating vertex shader");
    if (loadShader(&vertexShaderUI, "ui.vert", GL_VERTEX_SHADER) < 0) cleanUpAndExit(EXIT_FAILURE, "Error creating vertex shader for UI");
    if (loadShader(&vertexShaderDepth, "depth.vert", GL_VERTEX_SHADER) < 0) cleanUpAndExit(EXIT_FAILURE, "Error creating vertex shader for depth map");
    if (loadShader(&geometryShaderDepth, "depth.geom", GL_GEOMETRY_SHADER) < 0) cleanUpAndExit(EXIT_FAILURE, "Error creating geometry shader for depth map");
    if (loadShader(&fragmentShader, "fragment.frag", GL_FRAGMENT_SHADER) < 0) cleanUpAndExit(EXIT_FAILURE, "Error creating fragment shader");
    if (loadShader(&fragmentShaderUI, "ui.frag", GL_FRAGMENT_SHADER) < 0) cleanUpAndExit(EXIT_FAILURE, "Error creating fragment shader for UI");
    if (loadShader(&fragmentShaderLight, "light.frag", GL_FRAGMENT_SHADER) < 0) cleanUpAndExit(EXIT_FAILURE, "Error creating fragment shader for light");
    if (loadShader(&fragmentShaderDepth, "depth.frag", GL_FRAGMENT_SHADER) < 0) cleanUpAndExit(EXIT_FAILURE, "Error creating fragment shader for depth map");

    // Shader programs
    if (initShaderProgram(&shaderProgram, 2, vertexShader, fragmentShader) < 0) cleanUpAndExit(EXIT_FAILURE, "Error creating shader program");
    if (initShaderProgram(&shaderProgramLight, 2, vertexShader, fragmentShaderLight) < 0) cleanUpAndExit(EXIT_FAILURE, "Error creating shader program for light");
    if (initShaderProgram(&shaderProgramUI, 2, vertexShaderUI, fragmentShaderUI) < 0) cleanUpAndExit(EXIT_FAILURE, "Error creating shader program for UI");
    if (initShaderProgram(&shaderProgramDepth, 3, vertexShaderDepth, geometryShaderDepth, fragmentShaderDepth) < 0) cleanUpAndExit(EXIT_FAILURE, "Error creating shader program for depth map");
    
    // Delete now useless shaders
    destroyShader(&vertexShader);
    memset(&vertexShader, 0, sizeof(Shader));
    destroyShader(&vertexShaderUI);
    memset(&vertexShaderUI, 0, sizeof(Shader));
    destroyShader(&vertexShaderDepth);
    memset(&vertexShaderDepth, 0, sizeof(Shader));
    destroyShader(&fragmentShader);
    memset(&fragmentShader, 0, sizeof(Shader));
    destroyShader(&fragmentShaderUI);
    memset(&fragmentShaderUI, 0, sizeof(Shader));
    destroyShader(&fragmentShaderLight);
    memset(&fragmentShaderLight, 0, sizeof(Shader));
    destroyShader(&fragmentShaderDepth);
    memset(&fragmentShaderDepth, 0, sizeof(Shader));


    // Load SDL Mixer
    if (initMixer(AUDIO_NUMCHANS) < 0)
        cleanUpAndExit(EXIT_FAILURE, "Error initialising Mixer : %s", Mix_GetError());
    else mixerInitalized = 1;


    /* --- Load game objects --- */

    // Camera
    Bindings bindings = {
        SDL_SCANCODE_H,  // Forward
        SDL_SCANCODE_B,  // Backward
        SDL_SCANCODE_V,  // Left
        SDL_SCANCODE_N,  // Right
        SDL_SCANCODE_G,  // Sprint
        SDL_SCANCODE_SPACE,  // Jump
        SDL_SCANCODE_U,  // Use
        SDL_SCANCODE_I  // Reload
    };
    initCamera(&camera, (vec3){-2.0f, EYE_Y, 2.0f}, (vec3){-1.0f, EYE_Y, 2.0f}, &bindings);

    // Point lights
    if (initPointLight(&pointLights[0], (vec3){0.0f, 2.0f, 2.0f}, (vec3){0.9f, 0.9f, 0.9f})<0) cleanUpAndExit(EXIT_FAILURE, "Error creating point light");
    if (initPointLight(&pointLights[1], (vec3){2.3f, 3.3f, -4.0f}, (vec3){0.8f, 0.1f, 0.1f})<0) cleanUpAndExit(EXIT_FAILURE, "Error creating point light");
    if (initPointLight(&pointLights[2], (vec3){-4.0f, 2.0f, -12.0f}, (vec3){0.0f, 0.85f, 0.15f})<0) cleanUpAndExit(EXIT_FAILURE, "Error creating point light");
    if (initPointLight(&pointLights[3], (vec3){3.3f, 4.0f, -1.5f}, (vec3){0.1f, 0.0f, 0.95f})<0) cleanUpAndExit(EXIT_FAILURE, "Error creating point light");

    // Load textures
    // Temporary way to load textures
    Texture brickwall;
    loadTexture(&brickwall, "brickwall/baseColor.bmp", 4, TEXTURE_REPEAT, TEXTURE_BASECOLOR);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, brickwall.id);
    glUseProgram(shaderProgram);
    glUniform1i(glGetUniformLocation(shaderProgram, "textureSampler"), 0);
    glUseProgram(0);
    glActiveTexture(GL_TEXTURE1);

    // Vertices for a cube
    float vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 0.0f,  0.0f, -1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,  0.0f, -1.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,  0.0f, -1.0f, 
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  0.0f, -1.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.0f,  0.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  0.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,  0.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,  0.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.0f,  0.0f, 1.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, -1.0f,  0.0f,  0.0f,

        0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 1.0f,  0.0f,  0.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.0f,  0.0f,  0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f,  0.0f,  0.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.0f,  0.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 1.0f,  0.0f,  0.0f,
        0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f, -1.0f,  0.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 0.0f, -1.0f,  0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f, -1.0f,  0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f, -1.0f,  0.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  1.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  1.0f,  0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,  1.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 0.0f,  1.0f,  0.0f
    };
    unsigned int indices[] = {
        0, 1, 2, // first triangle
        1, 0, 6  // second triangle
    };

    // Bind data to buffers

    // Cubes
    glBindVertexArray(VAO);

    // Give data to buffers
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);


    // Light
    glBindVertexArray(lightVAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);


    // UI
    float uiVertices[] = {
        -1.0f, -1.0f, 0.0f,  // Bottom left
         1.0f,  1.0f, 0.0f,  // Top right
        -1.0f,  1.0f, 0.0f,  // Top left
        -1.0f, -1.0f, 0.0f,  // Bottom left
         1.0f, -1.0f, 0.0f,  // Bottom right
         1.0f,  1.0f, 0.0f,  // Top right
    };

    glBindVertexArray(uiVAO);

    glBindBuffer(GL_ARRAY_BUFFER, uiVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(uiVertices), uiVertices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);


    // Unbinding buffers
    glBindVertexArray(0);


    /* --- Main loop --- */

    Uint64 last_frame = SDL_GetTicks64();
    Uint64 now;
    Uint16 dt_ms;
    double dt;
    const Uint8* keyboardState = SDL_GetKeyboardState(NULL);
    bool pause = 0;
    bool quit = 0;
    SDL_Event e;
    while (!quit)
    {
        // Timing
        now = SDL_GetTicks64();
        dt_ms = now - last_frame;
        // Don't run the game faster than 500 FPS
        // This limit will be removed when SDL3 is released
        if (dt_ms < 2) {SDL_Delay(2-dt_ms); continue;}
        dt = dt_ms / (double)1000.0;
        last_frame = now;
        if (PRINT_FPS) printf("FPS : %lf\n", 1.0 / dt);  // Find a better way to print FPS

        // Handle events
        while (SDL_PollEvent(&e))
        {
            switch (e.type)
            {
                // Quit program
                case SDL_QUIT:
                    quit = 1;
                    break;
                // Handle key presses not related to camera
                case SDL_KEYDOWN:
                    switch (e.key.keysym.scancode)
                    {
                        case SDL_SCANCODE_F1:
                            quit = 1;
                            break;
                        case SDL_SCANCODE_ESCAPE:
                            pause = !pause;
                            SDL_ShowCursor(pause ? SDL_ENABLE : SDL_DISABLE);
                            break;
                        default:
                            break;
                    }
                    break;
                // Rotate camera
                case SDL_MOUSEMOTION:
                    // Power saving
                    if ((e.motion.xrel == 0 && e.motion.yrel == 0) || pause)
                        break;
                    rotateCamera(&camera, e.motion.xrel, e.motion.yrel);
                    SDL_WarpMouseInWindow(window, WINDOW_WIDTH/2, WINDOW_HEIGHT/2);
                    break;
                // Shoot
                case SDL_MOUSEBUTTONDOWN:
                    if (e.button.button == SDL_BUTTON_LEFT && !pause)
                        // TODO : Shoot
                        printf("Shoot !\n");
                    break;
                default:
                    break;
            }
        }
        if (!pause)
        {
            translateCamera(&camera, keyboardState, dt);
            updateCamera(&camera);
        }

        // Update game
        if (!pause) updateGame();

        // Render screen
        render(WINDOW_WIDTH, WINDOW_HEIGHT);
    }


    // Memory freeing and quitting
    cleanUp();
    
    return EXIT_SUCCESS;
}
