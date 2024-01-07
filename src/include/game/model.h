#ifndef MODEL_H
#define MODEL_H


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cglm/cglm.h>

#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <SDL2/SDL.h>

#include "textures.h"


#define MODELPATH "assets/models/"
#define MAXVERTICES 1000000


/**
 * @brief Vertex structure
 * 
 * @param position Position of the vertex
 * @param textureCoords Texture coordinates of the vertex
 * @param normal Normal of the vertex
*/
typedef struct {
    vec3 position;
    vec2 textureCoords;
    vec3 normal;
} Vertex;

/**
 * @brief Mesh structure
 * 
 * @param vertices Array of vertices
 * @param vertexCount Number of vertices
 * @param indices Array of indices
 * @param indexCount Number of indices
 * @param textures Array of textures
 * @param textureCount Number of textures
 * 
 * @param VAO Vertex Array Object
 * @param VBO Vertex Buffer Object
 * @param EBO Element Buffer Object
 * 
 * @note The VAO, VBO and EBO are used to draw the mesh and shouldn't be modified
 * @note The textures are loaded from the textures.h file
*/
typedef struct {
    Vertex *vertices;
    unsigned int vertexCount;
    unsigned int *indices;
    unsigned int indexCount;
    Texture *textures;
    unsigned int textureCount;

    unsigned int VAO;
    unsigned int VBO;
    unsigned int EBO;
} Mesh;


typedef struct {
    Mesh *meshes;
    unsigned int meshCount;
    char dir[64];
    Texture *texturesLoaded;
    unsigned int texturesLoadedCount;
} Model;


/**
 * @brief Draw a mesh
 * 
 * @param mesh Pointer to the mesh to draw
 * @param programShader The shader program to use
 * 
*/
void drawMesh(Mesh *mesh, unsigned int programShader);

/**
 * @brief Free a mesh
 * 
 * @param mesh Pointer to the mesh to free
*/
void freeMesh(Mesh *mesh);


/**
 * @brief Load a model from a file
 * 
 * @param model Pointer to the model to load
 * @param filename The name of the file to load
 * @return 0 on success, -1 on failure
 * 
 * @note The filename must be relative to the MODELPATH
*/
int loadModel(Model *model, char *filename);

/**
 * @brief Load a model from a file
 * 
 * @param model Pointer to the model to load
 * @param filename The name of the file to load
 * @return 0 on success, -1 on failure
 * 
 * @note The filename must be absolute
*/
int loadModelFullPath(Model *model, char *filename);

/**
 * @brief Draw a model
 * 
 * @param model Pointer to the model to draw
 * @param programShader The shader program to use
*/
void drawModel(Model *model, unsigned int programShader);

/**
 * @brief Free a model
 * 
 * @param model Pointer to the model to free
*/
void freeModel(Model *model);


#endif