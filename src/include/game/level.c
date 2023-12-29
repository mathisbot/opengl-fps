#include "level.h"


// Variables witnessing whether variables have been initialized or not
static Level* level = NULL;
static bool levelInitialized = 0;
static Enemy** enemies = NULL;
static bool enemiesInitialized = 0;
static int enemyInitializedIndex = 0;
static Texture** textures = NULL;
static bool texturesInitialized = 0;
static int textureInitializedIndex = 0;
static Wall** walls = NULL;
static bool wallsInitialized = 0;
static int wallInitializedIndex = 0;
static Point** points = NULL;
static bool pointsInitialized = 0;
static int pointInitializedIndex = 0;


static void cleanUp(const char* log, ...)
{
    va_list args;
    va_start(args, log);
    if (log)
        vfprintf(stderr, log, args);

    if (levelInitialized)
        free(level);
    if (enemiesInitialized)
    {
        for (int i = 0; i < enemyInitializedIndex; i++)
            freeEnemy(enemies[i]);
        free(enemies);
    }
    // TODO: free health packs
    // TODO: free ammo
    if (texturesInitialized)
    {
        for (int i = 0; i < textureInitializedIndex; i++)
            freeTexture(textures[i]);
        free(textures);
    }
    if (wallsInitialized)
    {
        for (int i = 0; i < wallInitializedIndex; i++)
            freeWall(walls[i]);
        free(walls);
    }
    if (pointsInitialized)
    {
        for (int i = 0; i < pointInitializedIndex; i++)
            freePoint(points[i]);
        free(points);
    }
    
}


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


Wall* createWall(Point** points, int pointCount, GLuint textureID)
{
    Wall* wall = malloc(sizeof(Wall));
    if (!wall)
    {
        fprintf(stderr, "Error allocating memory for wall\n");
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
    levelInitialized = 0;
    enemiesInitialized = 0;
    texturesInitialized = 0;
    wallsInitialized = 0;
    pointsInitialized = 0;

    // Allocating memory for Level structure
    level = (Level*)malloc(sizeof(Level));
    if (!level)
    {
        fprintf(stderr, "Error allocating memory for level\n");
        return NULL;
    }
    else
        levelInitialized = 1;

    // Opening file
    static char path[256];
    sprintf(path, "%slevel%d", LEVELPATH, levelNumber);
    FILE* file = fopen(path, "r");
    if (!file)
    {
        cleanUp("Error opening file %s\n", path);
        return NULL;
    }

    // Beyond this point, we assume that the file is correctly formatted
    // as checking for errors would be too time consuming
    // and useless most of the times (as level files are unlikely to change)
    
    // Reading start position
    // Start position is stored in the following format:
    // start: <x> <y> <z> <yaw> <pitch>
    static float x, y, z, yaw, pitch;
    fscanf(file, "start: %f %f %f %f %f\n", &x, &y, &z, &yaw, &pitch);
    level->startX = x;
    level->startY = y;
    level->startZ = z;
    level->startYaw = yaw;
    level->startPitch = pitch;

    // Reading enemies
    // Enemies are stored in the following format:
    // enemies: <enemyCount>
    // <ex> <ey> <ez> <eyaw> <epitch> <etype>
    // ...
    static int enemyCount;
    fscanf(file, "enemies: %d\n", &enemyCount);

    enemies = (Enemy**)malloc(sizeof(Enemy*)*enemyCount);
    if (!enemies)
    {
        fclose(file);
        cleanUp("Error allocating memory for enemies\n");
    }
    else
    {
        enemiesInitialized = 1;
        enemyInitializedIndex = 0;
    }
    static float ex, ey, ez, eyaw, epitch;
    static int etype;
    for (int i = 0; i < enemyCount; i++)
    {
        fscanf(file, "%f %f %f %f %f %d\n", &ex, &ey, &ez, &eyaw, &epitch, &etype);
        enemies[i] = createEnemy(ex, ey, ez, eyaw, epitch, etype);
        if (!enemies[i])
        {
            fclose(file);
            enemyInitializedIndex = i;
            cleanUp("Error creating enemy %d\n", i);
        }
    }

    level->enemies = enemies;
    level->enemyCount = enemyCount;

    // Reading health
    // Health packs are stored in the following format:
    // health: <healthCount>
    // <hx> <hy> <hz>
    // ...
    static int healthCount;
    fscanf(file, "health: %d\n", &healthCount);

    static float hx, hy, hz;
    for (int i = 0; i < healthCount; i++)
    {
        fscanf(file, "%f %f %f\n", &hx, &hy, &hz);
        // TODO: add health packs
        if (0)
        {
            fclose(file);
            // healthInitializedIndex = i;
            cleanUp("Error creating healthpack %d\n", i);
        }
    }

    // Reading ammo
    // Ammo packs are stored in the following format:
    // ammo: <ammoCount>
    // <ax> <ay> <az>
    // ...
    static int ammoCount;
    fscanf(file, "ammo: %d\n", &ammoCount);

    static float ax, ay, az;
    for (int i = 0; i < ammoCount; i++)
    {
        fscanf(file, "%f %f %f\n", &ax, &ay, &az);
        // TODO: add ammo packs
        if (0)
        {
            fclose(file);
            // ammoInitializedIndex = i;
            cleanUp("Error creating ammopack %d\n", i);
        }
    }

    // Reading textures to load in memory
    // Textures are stored in the following format:
    // textures: <textureCount>
    // <texturePath>
    // ...
    static int textureCount;
    fscanf(file, "textures: %d\n", &textureCount);

    textures = (Texture**)malloc(sizeof(Texture*)*textureCount);
    if (!textures)
    {
        fclose(file);
        cleanUp("Error allocating memory for textures\n");
    }
    else
    {
        texturesInitialized = 1;
        textureInitializedIndex = 0;
    }
    static char texturePath[128];
    for (int i = 0; i < textureCount; i++)
    {
        fscanf(file, "%s\n", texturePath);
        textures[i] = loadTexture(texturePath, TEXTURE_REPEAT);
        if (!textures[i])
        {
            fclose(file);
            textureInitializedIndex = i;
            cleanUp("Error loading texture %s\n", texturePath);
        }
    }

    level->textures = textures;
    level->textureCount = textureCount;

    // Reading walls
    // Walls are stored in the following format:
    // walls: <wallCount>
    // texture: <textureIndex>
    // pointcount: <pointCount>
    // <tex_x> <tex_y> <wx> <wy> <wz>
    // ...
    // ...
    static int wallCount;
    fscanf(file, "walls: %d\n", &wallCount);

    walls = (Wall**)malloc(sizeof(Wall*)*wallCount);
    if (!walls)
    {
        fclose(file);
        cleanUp("Error allocating memory for walls\n");
    }
    else
    {
        wallsInitialized = 1;
        wallInitializedIndex = 0;
    }
    static int textureIndex;
    static int pointCount;
    for (int i = 0; i < wallCount; i++)
    {
        pointsInitialized = 1;

        // Reading wall texture
        fscanf(file, "texture: %d\n", &textureIndex);

        if (textureIndex >= textureCount)
        {
            fclose(file);
            wallInitializedIndex = i;
            cleanUp("Error: texture index %d out of bounds\n", textureIndex);
        }

        // Reading points (vertices)
        fscanf(file, "pointcount: %d\n", &pointCount);

        points = (Point**)malloc(sizeof(Point*)*pointCount);
        if (!points)
        {
            fclose(file);
            wallInitializedIndex = i;
            cleanUp("Error allocating memory for points in wall %d\n", i);
        }
        else
        {
            pointsInitialized = 1;
            pointInitializedIndex = 0;
        }
        for (int j = 0; j < pointCount; j++)
        {
            static float tex_x, tex_y, wx, wy, wz;
            fscanf(file, "%f %f %f %f %f\n", &tex_x, &tex_y, &wx, &wy, &wz);
            points[j] = createPoint(wx, wy, wz, tex_x, tex_y);
            if (!points[j])
            {
                fclose(file);
                pointInitializedIndex = j;
                cleanUp("Error creating point %d in wall %d\n", j, i);
            }
        }

        walls[i] = createWall(points, pointCount, textures[textureIndex]->id);
        if (!walls[i])
        {
            fclose(file);
            wallInitializedIndex = i;
            cleanUp("Error creating wall %d\n", i);
        }
    }

    fclose(file);

    level->walls = walls;
    level->wallCount = wallCount;

    return level;
}

void freeLevel(Level* level)
{
    // Freeing enemies
    for (int i = 0; i < level->enemyCount; i++)
        freeEnemy(level->enemies[i]);
    free(level->enemies);

    // Freeing textures
    for (int i = 0; i < level->textureCount; i++)
        freeTexture(level->textures[i]);
    free(level->textures);

    // Freeing walls
    for (int i = 0; i < level->wallCount; i++)
        freeWall(level->walls[i]);
    free(level->walls);

    // Freeing level
    free(level);
}
