#ifndef SCENE_H
#define SCENE_H

#include "../Mathematics.h"
#include "Body.h"
#include "Shape.h"

class Collider {
public:
    Collider();
    void Step();
    Body* AddBody(Shape* shape, vec2 position);
    Body* AddBody(Shape* shape, float x, float y);
    void Render() const;
    void Clear();
    ~Collider();

    Body* RayCasting(const vec2& origin, const vec2& direction, const float& maxDistance) const;
    void CircleCasting(const vec2& origin, const float& radius) const;

private:
    std::vector<Body*> bodies;
};

#endif // SCENE_H
