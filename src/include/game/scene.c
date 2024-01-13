#include "scene.h"


void renderScene(const Scene *scene, GLuint programShader)
{
    // TODO: Draw models
    for (unsigned int i=0; i<scene->modelCount; i++) drawModel(&scene->models[i], programShader);

    // Temporary
     for (uint8_t i=0; i<10; i++)
    {
        // Model matrix
        static mat4 model = GLM_MAT4_IDENTITY_INIT;
        glm_mat4_identity(model);
        static vec3 position;
        glm_vec3_copy(scene->cubePositions[i], position);
        glm_translate(model, position);
        if (i==0) glm_scale(model, (vec3){30.0f, 0.01f, 30.0f});
        glm_rotate(model, glm_rad(20.0f*i), (vec3){0.5f, 1.0f, 0.0f});

        glUniformMatrix4fv(glGetUniformLocation(programShader, "model"), 1, GL_FALSE, (float*)model);

        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
}

void destroyScene(Scene *scene)
{
    for (unsigned int i=0; i<scene->modelCount; i++)
        freeModel(&scene->models[i]);
    free(scene->models);
}