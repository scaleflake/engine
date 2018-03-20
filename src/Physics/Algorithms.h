#ifndef COLLISION_H
#define COLLISION_H

#include "../Mathematics.h"
#include "Body.h"
#include "Shape.h"

class Collision;
class Body;

void ShapeToShape(Body* a, Body* b, Collision* collision);

#endif // COLLISION_H
