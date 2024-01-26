#include "shader.h"


static char* readSource(const char* filename)
{
    FILE* file = fopen(filename, "r");
    if (file == NULL)
    {
        LOG_ERROR("Failed to open file: %s\n", filename);
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* buffer = (char*)malloc(file_size + 1);
    if (buffer == NULL)
    {
        fclose(file);
        LOG_ERROR("Failed to allocate memory for file: %s\n", filename);
        return NULL;
    }

    size_t new_length = fread(buffer, sizeof(char), file_size, file);
    if (new_length == 0) {
        fclose(file);
        free(buffer);
        LOG_ERROR("Failed to read file: %s\n", filename);
        return NULL;
    }

    buffer[new_length] = '\0';
    fclose(file);
    return buffer;
    LOG_TRACE("Loaded shader source file %s\n", filename);
}


int loadShader(Shader *shader, const char *sourcePath, GLenum type)
{
    int success;
    char path[256];
    char infolog[512];

    snprintf(path, 255, "%s%s", SHADERPATH, sourcePath);

    shader->type = type;
    shader->id = glCreateShader(type);
    shader->source = readSource(path);
    if (shader->source == NULL) return -1;
    const GLchar* shaderSource[] = {shader->source};  // Inline ?
    glShaderSource(shader->id, 1, shaderSource, NULL);
    glCompileShader(shader->id);

    // Check for errors during compilation
    glGetShaderiv(shader->id, GL_COMPILE_STATUS, &success);
    if (success != GL_TRUE)
    {
        glGetShaderInfoLog(shader->id, 512, NULL, infolog);
        LOG_ERROR("Failed to compile shader %s: %s\n", path, infolog);
        destroyShader(shader);
        return -1;
    }
    return 0;
}

void destroyShader(Shader* shader)
{
    glDeleteShader(shader->id);
    free(shader->source);
}


int initShaderProgram(GLuint *prog, uint8_t shaderCount, ...)
{
    int success;
    char infolog[512];
    va_list args;
    va_start(args, shaderCount);

    *prog = glCreateProgram();
    Shader *shader = NULL;
    for (int i = 0; i < shaderCount; i++) {
        shader = va_arg(args, Shader*);
        glAttachShader(*prog, shader->id);
    }
    va_end(args);

    glLinkProgram(*prog);

    // Check for errors during linking
    glGetProgramiv(*prog, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(*prog, 512, NULL, infolog);
        LOG_ERROR("Failed to link shader program: %s\n", infolog);
        glDeleteProgram(*prog);
        *prog = 0;
        return -1;
    }

    LOG_DEBUG("Initialized shader program succesfully\n");

    return 0;
}