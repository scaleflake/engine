#include "../Precompiled.h"

Body::Body(Shape* shape, vec2 position) {
    this->shape = shape->Clone();
    this->shape->body = this;
    this->position = position;
    this->velocity = vec2(0.0f, 0.0f);
    this->angularVelocity = 0;
    this->torque = 0;
    this->orientation = Random(-CONST_PI, CONST_PI);
    this->force = vec2(0.0f, 0.0f);
    this->staticFriction = 0.5f;
    this->dynamicFriction = 0.3f;
    this->restitution = 0.2f;
    this->shape->Initialize();
}

void Body::SetStatic() {
    moment      = 0.0f;
    invMoment   = 0.0f;
    mass        = 0.0f;
    invMass     = 0.0f;
}

void Body::SetOrientation(float radians) {
    this->orientation = radians;
    this->shape->SetOrientation(radians);
}

Body::~Body() {

}
