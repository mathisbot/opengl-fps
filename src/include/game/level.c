#include "level.h"

Point* createPoint(float x, float y, float z, float tex_x, float tex_y)
{
    Point* point = malloc(sizeof(Point));
    if (!point)
    {
        printf("Error allocating memory for point\n");
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


Wall* createWall(Point** points, int pointCount, GLuint textureID)
{
    Wall* wall = malloc(sizeof(Wall));
    if (!wall)
    {
        printf("Error allocating memory for wall\n");
        exit(1);
    }

    wall->points = points;
    wall->pointCount = pointCount;
    wall->textureID = textureID;

    return wall;
}

void freeWall(Wall* wall)
{
    for (int i = 0; i < wall->pointCount; i++)
    {
        freePoint(wall->points[i]);
    }
    free(wall);
}


Level* loadLevel(uint16_t levelNumber)
{
    // Allocating memory for Level structure
    Level* level = malloc(sizeof(Level));
    if (!level)
    {
        printf("Error allocating memory for level\n");
        exit(1);
    }

    // Opening file
    char path[256];
    sprintf(path, "%slevel%d", LEVELPATH, levelNumber);
    FILE* file = fopen(path, "r");
    if (!file)
    {
        printf("Error opening file %s\n", path);
        free(level);
        fclose(file);
        return NULL;
    }
    
    // Reading start position
    float x, y, z, yaw, pitch;
    fscanf(file, "start: %f %f %f %f %f\n", &x, &y, &z, &yaw, &pitch);
    level->startX = x;
    level->startY = y;
    level->startZ = z;
    level->startYaw = yaw;
    level->startPitch = pitch;

    // Reading enemies
    int enemyCount;
    fscanf(file, "enemies: %d\n", &enemyCount);
    Enemy** enemies = (Enemy**)malloc(sizeof(Enemy*)*enemyCount);
    float ex, ey, ez, eyaw, epitch;
    int etype;
    for (int i = 0; i < enemyCount; i++)
    {
        fscanf(file, "%f %f %f %f %f %d\n", &ex, &ey, &ez, &eyaw, &epitch, &etype);
        enemies[i] = createEnemy(ex, ey, ez, eyaw, epitch, etype);
    }
    level->enemies = enemies;
    level->enemyCount = enemyCount;

    // Reading health
    int healthCount;
    fscanf(file, "health: %d\n", &healthCount);
    float hx, hy, hz;
    for (int i = 0; i < healthCount; i++)
    {
        fscanf(file, "%f %f %f\n", &hx, &hy, &hz);
    }


    // Reading ammo
    int ammoCount;
    fscanf(file, "ammo: %d\n", &ammoCount);
    float ax, ay, az;
    for (int i = 0; i < ammoCount; i++)
    {
        fscanf(file, "%f %f %f\n", &ax, &ay, &az);
    }

    // Reading textures
    int textureCount;
    fscanf(file, "textures: %d\n", &textureCount);
    Texture** textures = (Texture**)malloc(sizeof(Texture*)*textureCount);
    char texturePath[128];
    for (int i = 0; i < textureCount; i++)
    {
        char tpath[256];
        fscanf(file, "%s\n", texturePath);
        sprintf(tpath, "level%d/%s", levelNumber, texturePath);
        textures[i] = loadTexture(tpath, 0);
    }
    level->textures = textures;
    level->textureCount = textureCount;

    // Reading walls
    int wallCount;
    fscanf(file, "walls: %d\n", &wallCount);
    Wall** walls = (Wall**)malloc(sizeof(Wall*)*wallCount);
    int textureIndex;
    for (int i = 0; i < wallCount; i++)
    {
        // Reading wall texture
        fscanf(file, "texture: %d\n", &textureIndex);

        // Reading points (vertices)
        int pointCount;
        fscanf(file, "pointcount: %d\n", &pointCount);

        Point** points = (Point**)malloc(sizeof(Point*)*pointCount);
        for (int j = 0; j < pointCount; j++)
        {
            float tex_x, tex_y, wx, wy, wz;
            fscanf(file, "%f %f %f %f %f\n", &tex_x, &tex_y, &wx, &wy, &wz);
            points[j] = createPoint(wx, wy, wz, tex_x, tex_y);
        }

        if (textureIndex >= textureCount)
        {
            printf("Error: textureIndex out of bounds\n");
            exit(1);
        }

        walls[i] = createWall(points, pointCount, textures[textureIndex]->id);
    }
    level->walls = walls;
    level->wallCount = wallCount;

    return level;
}

void freeLevel(Level* level)
{
    // Freeing enemies
    for (int i = 0; i < level->enemyCount; i++)
    {
        freeEnemy(level->enemies[i]);
    }
    free(level->enemies);

    // Freeing textures
    for (int i = 0; i < level->textureCount; i++)
    {
        freeTexture(level->textures[i]);
    }
    free(level->textures);

    // Freeing walls
    for (int i = 0; i < level->wallCount; i++)
    {
        freeWall(level->walls[i]);
    }
    free(level->walls);

    // Freeing level
    free(level);
}
