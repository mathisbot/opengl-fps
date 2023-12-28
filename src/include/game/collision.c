#include "collision.h"


/**
 * @brief Creates a point in 3D space.
 * 
 * @param x The x coordinate of the point.
 * @param y The y coordinate of the point.
 * @param z The z coordinate of the point.
 * @return Point3D* A pointer to the point.
*/
Point3D* createPoint(float x, float y, float z)
{
    Point3D* point = malloc(sizeof(Point3D));
    if (point == NULL) {
        fprintf(stderr, "Error: malloc failed in createPoint()\n");
        return NULL;
    }
    point->x = x;
    point->y = y;
    point->z = z;
    return point;
}

/**
 * @brief Frees a point in 3D space.
 * 
 * @param point The point to free.
*/
void freePoint(Point3D* point)
{
    free(point);
}


/**
 * @brief Creates a vector in 3D space.
 * 
 * @param x The x coordinate of the vector.
 * @param y The y coordinate of the vector.
 * @param z The z coordinate of the vector.
 * @return Vector3D* A pointer to the vector.
*/
Vector3D* createVector(float x, float y, float z)
{
    Vector3D* vector = malloc(sizeof(Vector3D));
    if (vector == NULL) {
        fprintf(stderr, "Error: malloc failed in createVector()\n");
        return NULL;
    }
    vector->x = x;
    vector->y = y;
    vector->z = z;
    return vector;
}

/**
 * @brief Frees a vector in 3D space.
 * 
 * @param vector The vector to free.
*/
void freeVector(Vector3D* vector)
{
    free(vector);
}


/**
 * @brief Creates a quad in 3D space.
 * 
 * @param p1 The first point of the quad.
 * @param p2 The second point of the quad.
 * @param p3 The third point of the quad.
 * @param p4 The fourth point of the quad.
 * @return Quad* A pointer to the quad.
*/
Quad* createQuad(Point3D* p1, Point3D* p2, Point3D* p3, Point3D* p4)
{
    Quad* rect = malloc(sizeof(Quad));
    if (rect == NULL) {
        fprintf(stderr, "Error: malloc failed in createQuad()\n");
        return NULL;
    }
    rect->p[0] = p1;
    rect->p[1] = p2;
    rect->p[2] = p3;
    rect->p[3] = p4;
    return rect;
}

/**
 * @brief Frees a quad in 3D space.
 * 
 * @param rect The quad to free.
*/
void freeQuad(Quad* rect)
{
    free(rect);
}


/**
 * @brief Creates a collision box in 3D space.
 * 
 * @param p1 The first point of the box.
 * @param p2 The second point of the box.
 * @param p3 The third point of the box.
 * @param p4 The fourth point of the box.
 * @param p5 The fifth point of the box.
 * @param p6 The sixth point of the box.
 * @param p7 The seventh point of the box.
 * @param p8 The eighth point of the box.
 * @return CollisionBox* A pointer to the collision box.
*/
CollisionBox* createCollisionBox(Point3D* p1, Point3D* p2, Point3D* p3, Point3D* p4, Point3D* p5, Point3D* p6, Point3D* p7, Point3D* p8)
{
    CollisionBox* box = malloc(sizeof(CollisionBox));
    if (box == NULL) {
        fprintf(stderr, "Error: malloc failed in createCollisionBox()\n");
        return NULL;
    }
    box->p[0] = p1;
    box->p[1] = p2;
    box->p[2] = p3;
    box->p[3] = p4;
    box->p[4] = p5;
    box->p[5] = p6;
    box->p[6] = p7;
    box->p[7] = p8;
    return box;
}

/**
 * @brief Creates a collision box from a quad.
 * 
 * @param quad The quad to create the collision box from.
 * @param thickness The thickness of the collision box.
 * @return CollisionBox* A pointer to the collision box.
*/
CollisionBox* createCollisionBoxFromQuad(Quad* quad, float thickness)
{
    Point3D* p1 = createPoint(quad->p[0]->x+thickness, quad->p[0]->y+thickness, quad->p[0]->z+thickness);
    Point3D* p2 = createPoint(quad->p[0]->x-thickness, quad->p[0]->y-thickness, quad->p[0]->z-thickness);
    Point3D* p3 = createPoint(quad->p[1]->x+thickness, quad->p[1]->y+thickness, quad->p[1]->z+thickness);
    Point3D* p4 = createPoint(quad->p[1]->x-thickness, quad->p[1]->y-thickness, quad->p[1]->z-thickness);
    Point3D* p5 = createPoint(quad->p[2]->x+thickness, quad->p[2]->y+thickness, quad->p[2]->z+thickness);
    Point3D* p6 = createPoint(quad->p[2]->x-thickness, quad->p[2]->y-thickness, quad->p[2]->z-thickness);
    Point3D* p7 = createPoint(quad->p[3]->x+thickness, quad->p[3]->y+thickness, quad->p[3]->z+thickness);
    Point3D* p8 = createPoint(quad->p[3]->x-thickness, quad->p[3]->y-thickness, quad->p[3]->z-thickness);
    if (p1 == NULL || p2 == NULL || p3 == NULL || p4 == NULL || p5 == NULL || p6 == NULL || p7 == NULL || p8 == NULL) {
        fprintf(stderr, "Error: malloc failed in createCollisionBoxFromQuad()\n");
        free(p1);
        free(p2);
        free(p3);
        free(p4);
        free(p5);
        free(p6);
        free(p7);
        free(p8);
        return NULL;
    }
    CollisionBox* box = createCollisionBox(p1, p2, p3, p4, p5, p6, p7, p8);
    if (box == NULL) {
        fprintf(stderr, "Error: malloc failed in createCollisionBoxFromQuad()\n");
        free(p1);
        free(p2);
        free(p3);
        free(p4);
        free(p5);
        free(p6);
        free(p7);
        free(p8);
        return NULL;
    }
    return box;
}

/**
 * @brief Frees a collision box in 3D space.
 * 
 * @param box The collision box to free.
*/
void freeCollisionBox(CollisionBox* box)
{
    free(box);
}


/**
 * @brief Detects collision between two CollisionBox objects and returns the separation vector.
 * 
 * @param box1 The first CollisionBox.
 * @param box2 The second CollisionBox.
 * @return Vector3 The separation vector.
 */
Vector3D detectCollision(CollisionBox* box1, CollisionBox* box2)
{
    // Calculate the minimum and maximum coordinates of each box
    float min1X = box1->p[0]->x;
    float max1X = box1->p[0]->x;
    float min1Y = box1->p[0]->y;
    float max1Y = box1->p[0]->y;
    float min1Z = box1->p[0]->z;
    float max1Z = box1->p[0]->z;

    float min2X = box2->p[0]->x;
    float max2X = box2->p[0]->x;
    float min2Y = box2->p[0]->y;
    float max2Y = box2->p[0]->y;
    float min2Z = box2->p[0]->z;
    float max2Z = box2->p[0]->z;

    for (int i = 1; i < 8; i++) {
        if (box1->p[i]->x < min1X) min1X = box1->p[i]->x;
        if (box1->p[i]->x > max1X) max1X = box1->p[i]->x;
        if (box1->p[i]->y < min1Y) min1Y = box1->p[i]->y;
        if (box1->p[i]->y > max1Y) max1Y = box1->p[i]->y;
        if (box1->p[i]->z < min1Z) min1Z = box1->p[i]->z;
        if (box1->p[i]->z > max1Z) max1Z = box1->p[i]->z;

        if (box2->p[i]->x < min2X) min2X = box2->p[i]->x;
        if (box2->p[i]->x > max2X) max2X = box2->p[i]->x;
        if (box2->p[i]->y < min2Y) min2Y = box2->p[i]->y;
        if (box2->p[i]->y > max2Y) max2Y = box2->p[i]->y;
        if (box2->p[i]->z < min2Z) min2Z = box2->p[i]->z;
        if (box2->p[i]->z > max2Z) max2Z = box2->p[i]->z;
    }

    // Check for overlap in each dimension
    float overlapX = fmax(0, fmin(max1X, max2X) - fmax(min1X, min2X));
    float overlapY = fmax(0, fmin(max1Y, max2Y) - fmax(min1Y, min2Y));
    float overlapZ = fmax(0, fmin(max1Z, max2Z) - fmax(min1Z, min2Z));

    // Determine the smallest overlap
    float smallestOverlap = fmin(overlapX, fmin(overlapY, overlapZ));

    // Calculate the separation vector
    Vector3D separationVector;
    separationVector.x = (max1X + min1X) / 2 - (max2X + min2X) / 2;
    separationVector.y = (max1Y + min1Y) / 2 - (max2Y + min2Y) / 2;
    separationVector.z = (max1Z + min1Z) / 2 - (max2Z + min2Z) / 2;

    // Normalize the separation vector
    float length = sqrt(separationVector.x * separationVector.x + separationVector.y * separationVector.y + separationVector.z * separationVector.z);
    if (length > 0) {
        separationVector.x /= length;
        separationVector.y /= length;
        separationVector.z /= length;
    }

    // Scale the separation vector by the smallest overlap
    separationVector.x *= smallestOverlap;
    separationVector.y *= smallestOverlap;
    separationVector.z *= smallestOverlap;

    return separationVector;
}
