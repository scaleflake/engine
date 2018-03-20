#ifndef BODY_H
#define BODY_H

#include "../Mathematics.h"
#include "Shape.h"
class Shape;

struct Transform {
    vec2 position;
    float orientation;
};

enum BodyType { PrimalType, ShapedType, BouncyType };

class Body {
public:
    Body(Shape* shape, vec2 position);
    void SetStatic();
    bool IsStatic();
    void SetOrientation(float radians);
    ~Body();

    vec2 position;          // meters
    vec2 velocity;
    vec2 force;

    float orientation;      // radians
    float angularVelocity;
    float torque;

    float moment;       // moment of inertia
    float invMoment;    // inverse inertia
    float mass;         // mass
    float invMass;      // inverse masee

    float density;
    float restitution;
    float staticFriction;
    float dynamicFriction;

    Shape* shape;
};

#endif // BODY_H
