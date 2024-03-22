#include "collision.h"


bool pointInBox(vec3 point, BoxCollider box) {
    return point[0] >= box.position[0][0] && point[0] <= box.position[1][0] &&
           point[1] >= box.position[0][1] && point[1] <= box.position[3][1] &&
           point[2] >= box.position[0][2] && point[2] <= box.position[4][2];
}

bool pointInSphere(vec3 point, SphereCollider sphere) {
    return glm_vec3_distance2(point, sphere.position) <= sphere.radius * sphere.radius;
}

bool rayBoxIntersect(Ray ray, BoxCollider box) {
    vec3 invDirection = {1.0f / ray.direction[0], 1.0f / ray.direction[1], 1.0f / ray.direction[2]};

    float tmin = (box.position[0][0] - ray.origin[0]) * invDirection[0];
    float tmax = (box.position[1][0] - ray.origin[0]) * invDirection[0];

    float tymin = (box.position[0][1] - ray.origin[1]) * invDirection[1];
    float tymax = (box.position[3][1] - ray.origin[1]) * invDirection[1];

    if ((tmin > tymax) || (tymin > tmax)) return false;

    if (tymin > tmin) tmin = tymin;
    if (tymax < tmax) tmax = tymax;

    float tzmin = (box.position[0][2] - ray.origin[2]) * invDirection[2];
    float tzmax = (box.position[4][2] - ray.origin[2]) * invDirection[2];

    if ((tmin > tzmax) || (tzmin > tmax)) return false;

    if (tzmin > tmin) tmin = tzmin;
    if (tzmax < tmax) tmax = tzmax;

    return (tmin < ray.length) && (tmax > 0);
}

bool raySphereIntersect(Ray ray, SphereCollider sphere) {
    vec3 oc = {ray.origin[0] - sphere.position[0], ray.origin[1] - sphere.position[1], ray.origin[2] - sphere.position[2]};
    float a = glm_vec3_dot(ray.direction, ray.direction);
    float b = 2.0f * glm_vec3_dot(oc, ray.direction);
    float c = glm_vec3_dot(oc, oc) - sphere.radius * sphere.radius;
    float discriminant = b * b - 4 * a * c;

    if (discriminant < 0) return false;

    float t1 = (-b - sqrt(discriminant)) / (2 * a);
    float t2 = (-b + sqrt(discriminant)) / (2 * a);
    
    return (t1 > 0 && t1 < ray.length) || (t2 > 0 && t2 < ray.length);
}

bool boxBoxIntersect(BoxCollider box1, BoxCollider box2) {
    return box1.position[0][0] <= box2.position[1][0] && box1.position[1][0] >= box2.position[0][0] &&
           box1.position[0][1] <= box2.position[3][1] && box1.position[3][1] >= box2.position[0][1] &&
           box1.position[0][2] <= box2.position[4][2] && box1.position[4][2] >= box2.position[0][2];
}

bool sphereSphereIntersect(SphereCollider sphere1, SphereCollider sphere2) {
    return glm_vec3_distance2(sphere1.position, sphere2.position) <= (sphere1.radius + sphere2.radius) * (sphere1.radius + sphere2.radius);
}

bool boxSphereIntersect(BoxCollider box, SphereCollider sphere) {
    float x = glm_clamp(sphere.position[0], box.position[0][0], box.position[1][0]);
    float y = glm_clamp(sphere.position[1], box.position[0][1], box.position[3][1]);
    float z = glm_clamp(sphere.position[2], box.position[0][2], box.position[4][2]);

    float distance = glm_vec3_distance2((vec3){x, y, z}, sphere.position);

    return distance <= sphere.radius * sphere.radius;
}
