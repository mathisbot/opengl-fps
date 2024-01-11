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
        mesh->textureCount++;
        mesh->textures = (Texture*)realloc(mesh->textures, mesh->textureCount * sizeof(Texture));
        // Check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
        for (unsigned int j=0; j<model->texturesLoadedCount; j++)
        {
            if (!strcmp(model->texturesLoaded[j].path, path))
            {
                mesh->textureCount++;
                mesh->textures = (Texture*)realloc(mesh->textures, mesh->textureCount * sizeof(Texture));
                mesh->textures[*index] = model->texturesLoaded[j];
                skip = 1;
                break;
            }
        }
        // If texture hasn't been loaded already, load it
        if (!skip)
        {
            loadTextureFullPath(&mesh->textures[mesh->textureCount-1], path, 1, 0, type);  // From textures.h
            model->texturesLoadedCount++;
            model->texturesLoaded = (Texture*)realloc(model->texturesLoaded, model->texturesLoadedCount * sizeof(Texture));
            model->texturesLoaded[*index] = mesh->textures[mesh->textureCount-1];
        }
        *index += 1;
    }
}

static int processMesh(Model* model, Mesh* mesh, const struct aiMesh *aiMesh, const struct aiScene *scene)
{
    // Process vertices
    mesh->vertices = (Vertex*)malloc(aiMesh->mNumVertices * sizeof(Vertex));
    mesh->vertexCount = aiMesh->mNumVertices;
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
    }

    // Process indices
    mesh->indexCount = aiMesh->mNumFaces * 3;
    mesh->indices = (unsigned int*)malloc(aiMesh->mNumFaces * 3 * sizeof(unsigned int));
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

        unsigned int index = 0;
        loadMeshTexture(model, normalCount, material, mesh, aiTextureType_NORMAL_CAMERA, TEXTURE_NORMAL, &index);
        loadMeshTexture(model, diffuseCount, material, mesh, aiTextureType_DIFFUSE, TEXTURE_DIFFUSE, &index);
        loadMeshTexture(model, specularCount, material, mesh, aiTextureType_SPECULAR, TEXTURE_SPECULAR, &index);
        loadMeshTexture(model, heightCount, material, mesh, aiTextureType_HEIGHT, TEXTURE_HEIGHT, &index);
    }

    setupMesh(mesh);

    return 0;
}

void drawMesh(Mesh *mesh, unsigned int programShader)
{
    unsigned int diffuseNr = 0;
    unsigned int specularNr = 0;
    unsigned int normalNr = 0;
    unsigned int heightNr = 0;
    
    int textureUnits = 0;
    if (!textureUnits) glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &textureUnits);
    int textureStart = 1;  // Save GL_TEXTURE0 just in case

    // Bind appropriate textures
    int maxLoop = glm_min(textureUnits, mesh->textureCount);
    glUseProgram(programShader);
    for (unsigned int i=0; i<maxLoop; i++)
    {
        glActiveTexture(GL_TEXTURE0 + i+textureStart);
        char number[8];
        char name[32];
        char longName[64];
        if (mesh->textures[i].type == TEXTURE_DIFFUSE)
        {
            sprintf(name, "textureDiffuse%d", diffuseNr++);
            sprintf(number, "%d", diffuseNr++);
        }
        else if (mesh->textures[i].type == TEXTURE_SPECULAR)
        {
            sprintf(name, "textureSpecular%d", specularNr++);
            sprintf(number, "%d", specularNr++);
        }
        else if (mesh->textures[i].type == TEXTURE_NORMAL)
        {
            sprintf(name, "textureNormal%d", normalNr++);
            sprintf(number, "%d", normalNr++);
        }
        else if (mesh->textures[i].type == TEXTURE_HEIGHT)
        {
            sprintf(name, "textureHeight%d", heightNr++);
            sprintf(number, "%d", heightNr++);
        
        }
        sprintf(longName, "material.%s%s", name, number);
        glUniform1i(glGetUniformLocation(programShader, name), i+textureStart);
        glBindTexture(GL_TEXTURE_2D, mesh->textures[i].id);
    }
    glActiveTexture(GL_TEXTURE0);

    // Draw mesh
    glBindVertexArray(mesh->VAO);
    glDrawElements(GL_TRIANGLES, mesh->indexCount, GL_UNSIGNED_INT, 0);

    // Set everything back to defaults once configured
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(0);
    glUseProgram(0);
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
    // Process all the node's meshes (if any)
    for (unsigned int i=0; i<node->mNumMeshes; i++)
    {
        const struct aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        processMesh(model, &model->meshes[*index + i], mesh, scene);
    }
    *index += node->mNumMeshes;
    // Then do the same for each of its children
    for (unsigned int i=0; i<node->mNumChildren; i++) processNode(model, node->mChildren[i], scene, index);
}

static int loadFileIntoModel(Model *model, char *path)
{
    const struct aiScene *scene = aiImportFile(path, aiProcess_FlipUVs | aiProcessPreset_TargetRealtime_MaxQuality);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        LOG_ERROR("Error from Assimp when loading %s : %s\n", path, aiGetErrorString());
        aiReleaseImport(scene);
        return -1;
    }

    model->meshCount = scene->mNumMeshes;
    model->meshes = (Mesh*)malloc(model->meshCount * sizeof(Mesh));
    model->texturesLoaded = NULL;  // Will be allocated via realloc for dynamic size
    model->texturesLoadedCount = 0;
    unsigned int index = 0;
    processNode(model, scene->mRootNode, scene, &index);

    LOG_TRACE("Loaded file into model\n");
    return 0;
}

int loadModel(Model *model, char *filename)
{
    char path[128];
    sprintf(path, "%s%s", MODELPATH, filename);

    return loadModelFullPath(model, path);
}

int loadModelFullPath(Model *model, char *path)
{
    getDirectory(path, model->dir);
    loadFileIntoModel(model, path);

    LOG_DEBUG("Loaded model\n");

    return 0;
}

void drawModel(Model *model, unsigned int programShader)
{
    for (unsigned int i=0; i<model->meshCount; i++) drawMesh(&model->meshes[i], programShader);
}

void freeModel(Model *model)
{
    for (unsigned int i=0; i<model->meshCount; i++) freeMesh(&model->meshes[i]);
    free(model->meshes);
    for (unsigned int i=0; i<model->texturesLoadedCount; i++) destroyTexture(model->texturesLoaded[i]);
    free(model->texturesLoaded);
}