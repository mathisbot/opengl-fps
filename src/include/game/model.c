#include "model.h"


static inline void getDirectory(char* filePath, char* dir)
{
    int i;
    for (i=strlen(filePath)-1; i>=0; i--)
    {
        if (filePath[i] == '/' || filePath[i] == '\\')
        {
            strncpy(dir, filePath, i+1);
            dir[i+1] = '\0';
            break;
        }
    }
}


static void setupMesh(Mesh *mesh)
{
    // Create buffers/arrays
    glGenVertexArrays(1, &mesh->VAO);
    glGenBuffers(1, &mesh->VBO);
    glGenBuffers(1, &mesh->EBO);

    glBindVertexArray(mesh->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO);

    glBufferData(GL_ARRAY_BUFFER, mesh->vertexCount * sizeof(Vertex), mesh->vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->indexCount * sizeof(unsigned int), mesh->indices, GL_STATIC_DRAW);

    // Vertex positions
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);

    // Vertex texture coords
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, textureCoords));
    glEnableVertexAttribArray(1);

    // Vertex normals
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(2);

    // Vertex tangents
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));
    glEnableVertexAttribArray(3);

    // Vertex bitangents
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, bitangent));
    glEnableVertexAttribArray(4);

    glBindVertexArray(0);
}


static inline void loadMeshTexture(Model* model, unsigned int count, struct aiMaterial *material, Mesh *mesh, enum aiTextureType aiType, uint8_t type, unsigned int *index)
{
    for (unsigned int i=0; i<count; i++)
    {
        struct aiString str;
        aiGetMaterialTexture(material, aiType, i, &str, NULL, NULL, NULL, NULL, NULL, NULL);
        char path[1087];  // Avoid compiler warning
        sprintf(path, "%s%s", model->dir, str.data);
        bool skip = 0;
        // Check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
        for (unsigned int j=0; j<model->texturesLoadedCount; j++)
        {
            if (!strcmp(model->texturesLoaded[j].path, path))
            {
                mesh->textures[*index] = model->texturesLoaded[j];
                skip = 1;
                LOG_TRACE("Texture %s already loaded\n", path);
                break;
            }
        }
        // If texture hasn't been loaded already, load it
        if (!skip)
        {
            loadTextureFullPath(&mesh->textures[*index], path, 1, 0, type);  // From textures.h
            model->texturesLoadedCount++;
            // Dynamic size in O(1) (on average)
            if (model->texturesLoadedCount > model->texturesLoadedSize)
            {
                model->texturesLoadedSize *= 2;
                model->texturesLoaded = (Texture*)realloc(model->texturesLoaded, model->texturesLoadedSize * sizeof(Texture));
            }
            model->texturesLoaded[model->texturesLoadedCount-1] = mesh->textures[*index];
        }
        *index += 1;
    }
}

static int processMesh(Model* model, Mesh* mesh, const struct aiMesh *aiMesh, const struct aiScene *scene)
{
    // Process vertices
    mesh->vertexCount = aiMesh->mNumVertices;
    mesh->vertices = (Vertex*)malloc(mesh->vertexCount * sizeof(Vertex));
    for (unsigned int i=0; i<aiMesh->mNumVertices; i++)
    {
        // Position
        mesh->vertices[i].position[0] = aiMesh->mVertices[i].x;
        mesh->vertices[i].position[1] = aiMesh->mVertices[i].y;
        mesh->vertices[i].position[2] = aiMesh->mVertices[i].z;

        // Normal
        mesh->vertices[i].normal[0] = aiMesh->mNormals[i].x;
        mesh->vertices[i].normal[1] = aiMesh->mNormals[i].y;
        mesh->vertices[i].normal[2] = aiMesh->mNormals[i].z;

        // Texture coordinates
        if (aiMesh->mTextureCoords[0])
        {
            mesh->vertices[i].textureCoords[0] = aiMesh->mTextureCoords[0][i].x;
            mesh->vertices[i].textureCoords[1] = aiMesh->mTextureCoords[0][i].y;
        }
        else
        {
            mesh->vertices[i].textureCoords[0] = 0.0f;
            mesh->vertices[i].textureCoords[1] = 0.0f;
        }

        // Tangent
        mesh->vertices[i].tangent[0] = aiMesh->mTangents[i].x;
        mesh->vertices[i].tangent[1] = aiMesh->mTangents[i].y;
        mesh->vertices[i].tangent[2] = aiMesh->mTangents[i].z;

        // Bitangent
        mesh->vertices[i].bitangent[0] = aiMesh->mBitangents[i].x;
        mesh->vertices[i].bitangent[1] = aiMesh->mBitangents[i].y;
        mesh->vertices[i].bitangent[2] = aiMesh->mBitangents[i].z;
    }

    // Process indices
    mesh->indexCount = 0;
    for (unsigned int i=0; i<aiMesh->mNumFaces; i++) mesh->indexCount += aiMesh->mFaces[i].mNumIndices;
    mesh->indices = (unsigned int*)malloc(mesh->indexCount * sizeof(unsigned int));
    for (unsigned int i=0; i<aiMesh->mNumFaces; i++)
    {
        struct aiFace face = aiMesh->mFaces[i];
        for (unsigned int j=0; j<face.mNumIndices; j++) mesh->indices[i*3+j] = face.mIndices[j];
    }

    // Process material
    if (aiMesh->mMaterialIndex >= 0)
    {
        struct aiMaterial *material = scene->mMaterials[aiMesh->mMaterialIndex];
        unsigned int normalCount = aiGetMaterialTextureCount(material, aiTextureType_NORMAL_CAMERA);
        unsigned int diffuseCount = aiGetMaterialTextureCount(material, aiTextureType_DIFFUSE);
        unsigned int specularCount = aiGetMaterialTextureCount(material, aiTextureType_SPECULAR);
        unsigned int heightCount = aiGetMaterialTextureCount(material, aiTextureType_HEIGHT);
        mesh->textureCount = normalCount + diffuseCount + specularCount + heightCount;
        mesh->textures = (Texture*)malloc(mesh->textureCount * sizeof(Texture));

        LOG_TRACE("Mesh has %d textures\n", mesh->textureCount);
        LOG_TRACE("Mesh has %d normal textures\n", normalCount);
        LOG_TRACE("Mesh has %d diffuse textures\n", diffuseCount);
        LOG_TRACE("Mesh has %d specular textures\n", specularCount);
        LOG_TRACE("Mesh has %d height textures\n", heightCount);

        unsigned int index = 0;
        loadMeshTexture(model, normalCount, material, mesh, aiTextureType_NORMAL_CAMERA, TEXTURE_NORMAL, &index);
        loadMeshTexture(model, diffuseCount, material, mesh, aiTextureType_DIFFUSE, TEXTURE_DIFFUSE, &index);
        loadMeshTexture(model, specularCount, material, mesh, aiTextureType_SPECULAR, TEXTURE_SPECULAR, &index);
        loadMeshTexture(model, heightCount, material, mesh, aiTextureType_HEIGHT, TEXTURE_NORMAL, &index);
    }

    LOG_TRACE("Mesh has %d vertices, %d indices and %d textures.\n", mesh->vertexCount, mesh->indexCount, mesh->textureCount);

    setupMesh(mesh);

    return 0;
}

void drawMesh(Mesh *mesh, unsigned int programShader)
{
    unsigned int diffuseNr = 0;
    unsigned int specularNr = 0;
    unsigned int normalNr = 0;
    unsigned int heightNr = 0;
    
    // Program shader is bound before calling this function
    // glUseProgram(programShader);

    static int textureUnits = 0;
    static unsigned int maxLoop = 0;
    if (!textureUnits)
    {
        glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &textureUnits);
        maxLoop = glm_min(textureUnits, mesh->textureCount);
    }
    static const int textureStart = 1;  // Save GL_TEXTURE0 just in case

    // Bind appropriate textures
    for (unsigned int i=0; i<maxLoop; i++)
    {
        glActiveTexture(GL_TEXTURE0 + i+textureStart);
        char number[3];  // May be useful later
        char name[52];
        char longName[64];
        if (mesh->textures[i].type == TEXTURE_DIFFUSE)
        {
            sprintf(name, "diffuseMap");
            sprintf(number, "%d", diffuseNr++);
        }
        else if (mesh->textures[i].type == TEXTURE_SPECULAR)
        {
            sprintf(name, "specularMap");
            sprintf(number, "%d", specularNr++);
        }
        else if (mesh->textures[i].type == TEXTURE_NORMAL)
        {
            sprintf(name, "normalMap");
            sprintf(number, "%d", normalNr++);
        }
        else if (mesh->textures[i].type == TEXTURE_HEIGHT)
        {
            sprintf(name, "heightMap");
            sprintf(number, "%d", heightNr++);
        }
        sprintf(longName, "material.%s", name);
        glUniform1i(glGetUniformLocation(programShader, longName), i+textureStart);
        glBindTexture(GL_TEXTURE_2D, mesh->textures[i].id);
    }
    glActiveTexture(GL_TEXTURE0);

    // Draw mesh
    glBindVertexArray(mesh->VAO);
    glDrawElements(GL_TRIANGLES, mesh->indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void freeMesh(Mesh *mesh)
{
    free(mesh->vertices);
    free(mesh->indices);
    free(mesh->textures);
    glDeleteVertexArrays(1, &mesh->VAO);
    glDeleteBuffers(1, &mesh->VBO);
    glDeleteBuffers(1, &mesh->EBO);
}


static void processNode(Model *model, const struct aiNode *node, const struct aiScene *scene, unsigned int *index)
{
    // Process all the node's meshes
    for (unsigned int i=0; i<node->mNumMeshes; i++)
    {
        const struct aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        LOG_TRACE("Processing mesh %d\n", *index + i, mesh->mName.data);
        processMesh(model, &model->meshes[*index + i], mesh, scene);
    }
    *index += node->mNumMeshes;
    // Then do the same for each of its children
    for (unsigned int i=0; i<node->mNumChildren; i++) processNode(model, node->mChildren[i], scene, index);
}

static int loadFileIntoModel(Model *model, char *path, bool flipUVs)
{
    #if DEBUG
    Uint64 importStart = SDL_GetTicks64();
    #endif

    enum aiPostProcessSteps steps = aiProcess_OptimizeGraph | aiProcessPreset_TargetRealtime_MaxQuality;
    if (flipUVs) steps |= aiProcess_FlipUVs;
    const struct aiScene *scene = aiImportFile(path, steps);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE)
    {
        LOG_ERROR("Error from Assimp when loading %s : %s\n", path, aiGetErrorString());
        aiReleaseImport(scene);
        return -1;
    }
    LOG_TRACE("Loaded file %s into scene\n", path);

    model->meshCount = scene->mNumMeshes;
    model->meshes = (Mesh*)malloc(model->meshCount * sizeof(Mesh));
    model->texturesLoadedSize = 3;
    model->texturesLoaded = (Texture*)malloc(model->texturesLoadedSize * sizeof(Texture));  // Will be allocated via realloc for dynamic size
    model->texturesLoadedCount = 0;
    unsigned int index = 0;

    LOG_TRACE("Ready to process %d meshes\n", model->meshCount);

    processNode(model, scene->mRootNode, scene, &index);

    aiReleaseImport(scene);

    #if DEBUG
    Uint64 importEnd = SDL_GetTicks64();
    LOG_DEBUG("Imported model %s in %llu ms\n", path, importEnd-importStart);
    #endif

    return 0;
}

int loadModel(Model *model, char *filename, vec3 position, vec3 scale, vec3 rotation_vector, float rotation_angle, bool flipUVs)
{
    char path[128];
    snprintf(path, 127, "%s%s", MODELPATH, filename);

    return loadModelFullPath(model, path, position, scale, rotation_vector, rotation_angle, flipUVs);
}

int loadModelFullPath(Model *model, char *path, vec3 position, vec3 scale, vec3 rotation_vector, float rotation_angle, bool flipUVs)
{
    getDirectory(path, model->dir);
    loadFileIntoModel(model, path, flipUVs);

    glm_vec3_copy(position, model->position);
    glm_vec3_copy(scale, model->scale);
    glm_vec3_copy(rotation_vector, model->rotation_vector);
    model->rotation_angle = rotation_angle;

    return 0;
}

void drawModel(Model *model, unsigned int programShader)
{
    static mat4 modelMat = GLM_MAT4_IDENTITY_INIT;
    glm_mat4_identity(modelMat);
    glm_translate(modelMat, model->position);
    glm_scale(modelMat, model->scale);
    glm_rotate(modelMat, model->rotation_angle, model->rotation_vector);
    glUniformMatrix4fv(glGetUniformLocation(programShader, "model"), 1, GL_FALSE, (float*)modelMat);

    for (unsigned int i=0; i<model->meshCount; i++) drawMesh(&model->meshes[i], programShader);
}

void freeModel(Model *model)
{
    for (unsigned int i=0; i<model->meshCount; i++) freeMesh(&model->meshes[i]);
    free(model->meshes);
    for (unsigned int i=0; i<model->texturesLoadedCount; i++) destroyTexture(model->texturesLoaded[i]);
    free(model->texturesLoaded);
}