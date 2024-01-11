#ifndef SHADER_H
#define SHADER_H


#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#include <GL/glew.h>

#include "logs.h"


#define SHADERPATH "assets/shaders/"


typedef struct {
    GLuint id;
    char *source;
    GLenum type;
} Shader;


/**
 * @brief Loads a shader from a file
 * 
 * @param filename Path to the source file
 * @param sourcePath Path to the source file
 * @param type Type of the shader
 * @return int 0 if success, -1 if error
*/
int loadShader(Shader *shader, const char *sourcePath, GLenum type);

/**
 * @brief Destroy a shader object
 * 
 * @param shader Pointer to the shader object
*/
void destroyShader(Shader* shader);

/**
 * @brief Create a shader program
 * 
 * @param prog Pointer to the program object
 * @param shaderCount Number of shaders
 * @param ... Pointers to shaders
 * @return int 0 if success, -1 if error
*/
int initShaderProgram(GLuint *prog, uint8_t shaderCount, ...);

#endif // SHADER_H
