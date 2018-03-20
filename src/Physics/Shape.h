#ifndef SHAPE_H
#define SHAPE_H

#include "../Mathematics.h"
#include "Body.h"
class Body;

#define MaxPolyVertexCount 8

enum ShapeType { CIRCLE_TYPE, POLYGON_TYPE };

class Shape  {
public:
    Shape() {}
    virtual ~Shape() {}

    virtual Shape* Clone() const = 0;
    virtual void Initialize() = 0;
    virtual void ComputeMass(float density) = 0;
    virtual void SetOrientation(float radians) = 0;
    virtual void Draw() const = 0;
    virtual ShapeType GetType() const = 0;


    Body* body;
    mat2 orientation; // from model to world
};

class Circle : public Shape {
public:
    Circle(float r);
    ~Circle();

    Shape* Clone() const;
    void Initialize();
    void ComputeMass(float density);
    void SetOrientation(float radians);
    void Draw() const;
    ShapeType GetType() const;



    float radius;
};

class Polygon : public Shape {
public:
    Polygon(vec2* vertices, uint count);
    Polygon(float width, float height);
    ~Polygon();

    Shape* Clone() const;
    void Initialize();
    void ComputeMass(float density);
    void SetOrientation(float radians);
    void Draw() const;
    ShapeType GetType() const;


    vec2 GetSupport(vec2 dir) const;

    float radius;
    uint vertexCount;
    vec2 vertices[MaxPolyVertexCount];
    vec2 normals[MaxPolyVertexCount];
};

#endif // SHAPE_H
