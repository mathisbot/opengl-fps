#ifndef COLLISION_H
#define COLLISION_H


#include <cglm/cglm.h>


/**
 * @brief Box collider structure
 * 
 * @param position Position of the 8 corners of the box
*/
typedef struct {
    vec3 position[8];
} BoxCollider;

/**
 * @brief Sphere collider structure
 * 
 * @param position Position of the sphere
 * @param radius Radius of the sphere
*/
typedef struct {
    vec3 position;
    float radius;
} SphereCollider;

/**
 * @brief Ray structure
 * 
 * @param origin Origin of the ray
 * @param direction Direction of the ray
*/
typedef struct {
    vec3 origin;
    vec3 direction;
    float length;
} Ray;

/**
 * @brief Check if a point is inside a box
 * 
 * @param point Point to check
 * @param box Box to check
 * @return true Point is inside the box
 * @return false Point is not inside the box
*/
bool pointInBox(vec3 point, BoxCollider box);

/**
 * @brief Check if a point is inside a sphere
 * 
 * @param point Point to check
 * @param sphere Sphere to check
 * @return true Point is inside the sphere
 * @return false Point is not inside the sphere
*/
bool pointInSphere(vec3 point, SphereCollider sphere);

/**
 * @brief Check if a ray intersects a box
 * 
 * @param ray Ray to check
 * @param box Box to check
 * @return true Ray intersects the box
 * @return false Ray does not intersect the box
*/
bool rayBoxIntersect(Ray ray, BoxCollider box);

/**
 * @brief Check if a ray intersects a sphere
 * 
 * @param ray Ray to check
 * @param sphere Sphere to check
 * @return true Ray intersects the sphere
 * @return false Ray does not intersect the sphere
*/
bool raySphereIntersect(Ray ray, SphereCollider sphere);

/**
 * @brief Check if a box intersects another box
 * 
 * @param box1 First box
 * @param box2 Second box
 * @return true Boxes intersect
 * @return false Boxes do not intersect
*/
bool boxBoxIntersect(BoxCollider box1, BoxCollider box2);

/**
 * @brief Check if a sphere intersects another sphere
 * 
 * @param sphere1 First sphere
 * @param sphere2 Second sphere
 * @return true Spheres intersect
 * @return false Spheres do not intersect
*/
bool sphereSphereIntersect(SphereCollider sphere1, SphereCollider sphere2);

/**
 * @brief Check if a box intersects a sphere
 * 
 * @param box Box
 * @param sphere Sphere
 * @return true Box intersects the sphere
 * @return false Box does not intersect the sphere
*/
bool boxSphereIntersect(BoxCollider box, SphereCollider sphere);

#endif
