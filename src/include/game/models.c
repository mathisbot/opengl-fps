#include "models.h"





Point* createPoint(float x, float y, float z, float tex_x, float tex_y)
{
    Point* point = malloc(sizeof(Point));
    if (!point)
    {
        fprintf(stderr, "Error allocating memory for point\n");
        exit(1);
    }

    point->x = x;
    point->y = y;
    point->z = z;
    point->tex_x = tex_x;
    point->tex_y = tex_y;

    return point;
}

void freePoint(Point* point)
{
    free(point);
}


Model* createModel(Point* points, int num_points)
{
    Model* model = malloc(sizeof(Model));
    if (model == NULL) {
        fprintf(stderr, "Error: Could not allocate memory for Model object\n");
        return NULL;
    }
    model->points = points;
    model->num_points = num_points;
    return model;
}

static char path[256];

Model* loadModel(char* filename)
{
    int num_vertex;
    int num_textures;
    int num_points;

    sprintf(path, "%s%s", MODELPATH, filename);
    FILE* file = fopen(path, "r");
    if (file == NULL) {
        fprintf(stderr, "Error: Could not open model file %s\n", path);
        return NULL;
    }

    fscanf(file, "format: %d %d %d\n", &num_vertex, &num_textures, &num_points);

    float (*vertex)[3] = (float (*)[3])malloc(num_points*sizeof(float[3]));
    float (*textures)[3] = (float (*)[3])malloc(2*num_textures*sizeof(float[3]));
    Point* points = (Point*)malloc(num_points*sizeof(Point));

    float x, y, z, tex_x, tex_y;

    // Reading vertices
    for (int i=0; i<num_vertex; i++)
    {
        fscanf(file, "v %f %f %f\n", &x, &y, &z);
        vertex[i][0] = x;
        vertex[i][1] = y;
        vertex[i][2] = z;
    }

    // Reading texture coordinates
    for (int i=0; i<num_textures; i++)
    {
        fscanf(file, "vt %f %f\n", &tex_x, &tex_y);
        textures[i][0] = tex_x;
        textures[i][1] = tex_y;
    }

    // Reading points
    int pointIndex, textureIndex;
    for (int i = 0; i < num_points; i++)
    {
        // Number of coordinates per line is unknown
        char lineHeader[128]; 
        int numRead = fscanf(file, "%s", lineHeader);
        
        if (numRead == EOF)
            break;

        if (strcmp(lineHeader, "f") == 0)
        {
            while (fscanf(file, " %d/%d", &pointIndex, &textureIndex) > 0)
            {
                createPoint(vertex[pointIndex][0], vertex[pointIndex][1], vertex[pointIndex][2], textures[textureIndex][0], textures[textureIndex][1]);
                // Check if end of line
                char nextChar = fgetc(file);
                if (nextChar == '\n' || nextChar == EOF)
                    break;
                ungetc(nextChar, file);
            }
        }
    }

    fclose(file);

    return createModel(points, num_points);
}

void freeModel(Model* model)
{
    free(model->points);
    free(model);
}