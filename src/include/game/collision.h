#ifndef COLLISION_H
#define COLLISION_H


#include <stdio.h>
#include <stdbool.h>


typedef struct {
    float x;
    float y;
    float z;
} Point3D;

typedef struct {
    float x;
    float y;
    float z;
} Vector3D;

typedef struct {
    Point3D* p[4];
} Quad;

typedef struct {
    Point3D* p[8];
} CollisionBox;


Point3D* createPoint(float x, float y, float z);
void freePoint(Point3D* point);

Vector3D* createVector(float x, float y, float z);
void freeVector(Vector3D* vector);

Quad* createQuad(Point3D* p1, Point3D* p2, Point3D* p3, Point3D* p4);
void freeQuad(Quad* rect);

CollisionBox* createCollisionBox(Point3D* p1, Point3D* p2, Point3D* p3, Point3D* p4, Point3D* p5, Point3D* p6, Point3D* p7, Point3D* p8);
CollisionBox* createCollisionBoxFromQuad(Quad* quad, float thickness);
void freeCollisionBox(CollisionBox* box);


bool boxIntersect(CollisionBox box1, CollisionBox box2);


#endif