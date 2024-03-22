#include "light.h"


int createDepthCubemap(GLuint *depthCubemap, unsigned int resolution)
{
    glGenTextures(1, depthCubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, *depthCubemap);
    for (int i=0; i<6; i++)
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X+i, 0, GL_DEPTH_COMPONENT, resolution, resolution, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    LOG_TRACE("Created depth cubemap with resolution %d\n", resolution);
    return 0;
}

inline void bindDepthCubemapToFBO(GLuint depthMapFBO, GLuint depthCubemap)
{
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void destroyDepthCubemap(GLuint depthCubemap)
{
    glDeleteTextures(1, &depthCubemap);
}


int initPointLight(PointLight *light, vec3 position, vec3 color)
{
    glm_vec3_copy(position, light->position);
    glm_vec3_copy(color, light->color);
    if (createDepthCubemap(&(light->depthCubemap), SHADOWMAP_RES)<0)
    {
        LOG_ERROR("Could not create depth cubemap for point light\n");
        return -1;
    }
    LOG_TRACE("Initialized point light\n");
    return 0;
}


void renderPointLightsShadowMap(const Scene *scene, GLuint shaderProgramDepth, GLuint depthMapFBO, PointLight *pointLights)
{
    glViewport(0, 0, SHADOWMAP_RES, SHADOWMAP_RES);
    glUseProgram(shaderProgramDepth);

    glUniform1f(glGetUniformLocation(shaderProgramDepth, "farPlane"), SHADOWMAP_ZFAR);

    // We don't want to compute tan each tick
    static bool firstTime = true;
    static mat4 lightProjection = {0};
    if (firstTime) {glm_perspective(glm_rad(90.0f), 1.0f, SHADOWMAP_ZNEAR, SHADOWMAP_ZFAR, lightProjection); firstTime=false;}

    // Compute depth map for each light
    static mat4 shadowMatrices[6];
    for (uint8_t i=0; i<4; i++)
    {
        // Each light has its own depth cubemap
        bindDepthCubemapToFBO(depthMapFBO, pointLights[i].depthCubemap);

        glUniform3f(glGetUniformLocation(shaderProgramDepth, "lightPos"), pointLights[i].position[0], pointLights[i].position[1], pointLights[i].position[2]);

        pointLightGetProjMatrices(&(pointLights[i]), &lightProjection, &shadowMatrices);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgramDepth, "shadowMatrices"), 6, GL_FALSE, (float*)(shadowMatrices));

        // Rendering
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);

        renderScene(scene, shaderProgramDepth);
    }
}


void pointLightGetProjMatrices(PointLight *pointLight, mat4 *lightProjection, mat4 (*dest)[6])
{
    mat4 lightView[6];  // 6 faces of the cubemap
    vec3 lightTargets[6];
    glm_vec3_add(pointLight->position, (vec3){1.0f, 0.0f, 0.0f}, lightTargets[0]);
    glm_lookat(pointLight->position, lightTargets[0], (vec3){0.0f, -1.0f, 0.0f}, lightView[0]);
    glm_vec3_add(pointLight->position, (vec3){-1.0f, 0.0f, 0.0f}, lightTargets[1]);
    glm_lookat(pointLight->position, lightTargets[1], (vec3){0.0f, -1.0f, 0.0f}, lightView[1]);
    glm_vec3_add(pointLight->position, (vec3){0.0f, 1.0f, 0.0f}, lightTargets[2]);
    glm_lookat(pointLight->position, lightTargets[2], (vec3){0.0f, 0.0f, 1.0f}, lightView[2]);
    glm_vec3_add(pointLight->position, (vec3){0.0f, -1.0f, 0.0f}, lightTargets[3]);
    glm_lookat(pointLight->position, lightTargets[3], (vec3){0.0f, 0.0f, -1.0f}, lightView[3]);
    glm_vec3_add(pointLight->position, (vec3){0.0f, 0.0f, 1.0f}, lightTargets[4]);
    glm_lookat(pointLight->position, lightTargets[4], (vec3){0.0f, -1.0f, 0.0f}, lightView[4]);
    glm_vec3_add(pointLight->position, (vec3){0.0f, 0.0f, -1.0f}, lightTargets[5]);
    glm_lookat(pointLight->position, lightTargets[5], (vec3){0.0f, -1.0f, 0.0f}, lightView[5]);

    glm_mat4_mul(*lightProjection, lightView[0], (*dest)[0]);
    glm_mat4_mul(*lightProjection, lightView[1], (*dest)[1]);
    glm_mat4_mul(*lightProjection, lightView[2], (*dest)[2]);
    glm_mat4_mul(*lightProjection, lightView[3], (*dest)[3]);
    glm_mat4_mul(*lightProjection, lightView[4], (*dest)[4]);
    glm_mat4_mul(*lightProjection, lightView[5], (*dest)[5]);
}

void destroyPointLight(PointLight *light)
{
    destroyDepthCubemap(light->depthCubemap);
}