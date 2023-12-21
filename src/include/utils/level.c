#include "level.h"


Level* loadLevel(char* path)
{
    Level* level = malloc(sizeof(Level));
    if (!level)
    {
        printf("Error allocating memory for level\n");
        exit(1);
    }

    FILE* file = fopen(path, "r");
    if (!file)
    {
        printf("Error opening file %s\n", path);
        free(level);
        fclose(file);
        return NULL;
    }
    
    int n = 0;
    float x, y, z, yaw, pitch;

    // Counting the number of lines in the file
    fscanf(file, "%f %f %f %f %f", &x, &y, &z, &yaw, &pitch);
    float temp;
    while (fscanf(file, "%f", &temp) == 1)
        n++;

    fseek(file, 0, SEEK_SET);

    float* vertices = malloc(sizeof(float) * n);
    if (!vertices)
    {
        printf("Error allocating memory for vertices\n");
        free(level);
        fclose(file);
        exit(1);
    }

    // Reading the file
    int i = 0;
    fscanf(file, "%f %f %f", &x, &y, &z);
    while (fscanf(file, "%f", &temp) == 1)
        vertices[i++] = temp;

    fclose(file);

    level->startX = x;
    level->startY = y;
    level->startZ = z;
    level->startYaw = yaw;
    level->startPitch = pitch;
    level->walls = vertices;
    level->wallCount = n;

    return level;
}

void freeLevel(Level* level)
{
    free(level->walls);
    free(level);
}
