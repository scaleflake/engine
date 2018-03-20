#ifndef MANIFOLD_H
#define MANIFOLD_H

#include "../Mathematics.h"
#include "Body.h"
class Body;

class Collision {
public:
    Collision(Body* a, Body* b);

    void Solve();                   // Generate contact information
    void Initialize();              // Precalculations for impulse solving
    void ApplyImpulse();            // Solve impulse and apply
    void PositionalCorrection();    // Naive correction of positional penetration
    void Draw() const;

    float penetration;  // Depth of penetration from collision
    vec2 normal;        // From A to B
    vec2 contacts[2];   // Points of contact during collision
    byte contactCount;  // Number of contacts that occured during collision

private:
    Body* A;
    Body* B;

    float restitution;      // Mixed restitution
    float dynamicFriction;  // Mixed dynamic friction
    float staticFriction;   // Mixed static friction
};

#endif // MANIFOLD_H
