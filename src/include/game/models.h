#ifndef MODELS_H
#define MODELS_H


#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>


#define MODELPATH "assets/models/"


typedef struct {
    float x;
    float y;
    float z;
    float tex_x;
    float tex_y;
} Point;


typedef struct {
    Point* points;
    int num_points;
} Model;


/**
 * @brief Create a point
 * 
 * @param x X position of the point
 * @param y Y position of the point
 * @param z Z position of the point
 * @param tex_x X position of the texture
 * @param tex_y Y position of the texture
 * @return Point* Pointer to the point
 */
Point* createPoint(float x, float y, float z, float tex_x, float tex_y);

/**
 * @brief Free a point
 * 
 * @param point Pointer to the point
 */
void freePoint(Point* point);


/**
 * @brief Creates a model from points.
 * 
 * @param points The points to create the model from.
 * @param num_points The number of points.
 * @return Model* The created model.
*/
Model* createModel(Point* points, int num_points);

/**
 * @brief Creates a model from a file.
 * 
 * @param path The path to the model file.
 * @return Model* The created model.
*/
Model* loadModel(char* path);

/**
 * @brief Destroys a model.
 * 
 * @param model The model to destroy.
*/
void freeModel(Model* model);

#endif