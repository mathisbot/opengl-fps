#include "shader.h"


static char* readSource(const char* filename)
{
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Failed to open file: %s\n", filename);
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* buffer = (char*)malloc(file_size + 1);
    if (buffer == NULL) {
        fclose(file);
        fprintf(stderr, "Failed to allocate memory for file: %s\n", filename);
        return NULL;
    }

    size_t new_length = fread(buffer, sizeof(char), file_size, file);
    if (new_length == 0) {
        fclose(file);
        free(buffer);
        fprintf(stderr, "Failed to read file: %s\n", filename);
        return NULL;
    }

    buffer[new_length] = '\0';
    fclose(file);
    return buffer;
}


int loadShader(Shader *shader, const char *sourcePath, GLenum type)
{
    int success;
    char path[256];
    char infolog[512];

    sprintf(path, "%s%s", SHADERPATH, sourcePath);

    shader->type = type;
    shader->id = glCreateShader(type);
    shader->source = readSource(path);
    if (shader->source == NULL) return -1;
    const GLchar* shaderSource[] = {shader->source};  // Inline ?
    glShaderSource(shader->id, 1, shaderSource, NULL);
    glCompileShader(shader->id);

    // Check for errors during compilation
    glGetShaderiv(shader->id, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader->id, 512, NULL, infolog);
        fprintf(stderr, "Failed to compile shader %s: %s\n", path, infolog);
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