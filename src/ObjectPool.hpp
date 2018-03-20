#ifndef OBJECTPOOL_H
#define OBJECTPOOL_H

#include "Physics/Shape.h"

class ObjectPool {
public:
    ObjectPool() {}
    void Initialize();
    Circle* TakeCircle();
    Polygon* TakePolygon();
    void Release(Shape* shape);
private:
    vector<Shape*> shapes;
};

#endif // OBJECTPOOL_H
