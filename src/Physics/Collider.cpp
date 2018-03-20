#include "../Precompiled.h"

Collider::Collider() {

}

void Collider::Step() {
    // Integrate velocities
    for (uint i = 0; i < bodies.size(); ++i) {
        Body* body = bodies[i];

        if (body->invMass == 0.0f)
            continue;

        body->position += body->velocity * CONST_DELTATIME;
        body->velocity *= 0.99f;
        body->orientation += body->angularVelocity * CONST_DELTATIME;
        body->angularVelocity *= 0.99f;
        body->SetOrientation(body->orientation);
#ifdef GRAVITY
        body->velocity += (body->force * body->invMass + gravity) * CONST_DELTATIME;
#endif // GRAVITY
    }

    // Generate new collision info
    //collisions.clear();
    for (uint i = 0; i < bodies.size(); ++i) {
        Body* A = bodies[i];
        for (uint j = i + 1; j < bodies.size(); ++j) {
            Body* B = bodies[j];
            if (A->invMass == 0.0f and B->invMass == 0.0f)
                continue;
            Collision collision(A, B);
            collision.Solve();                  // Detect contact
//            if (collision.contactCount) collisions.emplace_back(collision);
            collision.Initialize();             // Initialize collision
            collision.ApplyImpulse();           // Solve collisions
            collision.PositionalCorrection();   // Correct positions
            collision.Draw();

        }
    }

    // Clear all forces
    for (uint i = 0; i < bodies.size(); ++i) {
        Body* body = bodies[i];
        body->force = vec2(0.0f, 0.0f);
        body->torque = 0;
    }
}

void Collider::Render() const {
    for (auto body : bodies) {
        body->shape->Draw();
    }

//    for (auto collision : collisions) {
//        collision.Draw();
//    }
}

Body* Collider::AddBody(Shape* shape, vec2 position) {
    assert(shape);
    Body* body = new Body(shape, position);
    bodies.push_back(body);
    return body;
}

Body* Collider::AddBody(Shape* shape, float x, float y) {
    return AddBody(shape, vec2(x, y));
}

void Collider::Clear() {
    for (auto body : bodies) {
        delete body->shape;
        delete body;
    }
    bodies.clear();
}

Collider::~Collider() {
    this->Clear();
}

//{ Casting
inline bool RayCrossesFace(const vec2& origin, const vec2& direction, const float& maxDistance, const vec2& a, const vec2& b, float& distance) {
    vec2 face = b - a;
    vec2 normal = vec2(face.y, -face.x);

    float penetration = glm::dot(direction, normal);
    if (penetration == 0.0f) {
        distance = CONST_MAXFLOAT;
        return false;
    }

    vec2 relative = a - origin;

    distance = glm::dot(normal, relative) / penetration;
    if (distance < 0.0f or distance > maxDistance) {
        return false;
    }

    float s = glm::dot(vec2(direction.y, -direction.x), relative) / penetration;
    if (s < 0.0f or s > 1.0f) {
        return false;
    }

    return true;
}

inline bool RayCrossesBody(const vec2& origin, const vec2& direction, const float& maxDistance, const Body& body, float& distance, vec2& point, vec2& normal ) {
    if (body.shape->GetType() == CIRCLE_TYPE) {
        Circle* circle = reinterpret_cast<Circle*>(body.shape);

        distance = CONST_MAXFLOAT;
        point = origin;
        normal = direction;

        vec2 relative = body.position - origin;

        float temp_dist = glm::dot(relative, vec2(direction.y, -direction.x));
        if (temp_dist < -circle->radius or temp_dist > circle->radius) {
            return false;
        }

        distance = glm::dot(relative, direction) - sqrt(circle->radius * circle->radius - temp_dist * temp_dist);
        if (distance < 0.0f or distance > maxDistance) {
            return false;
        }

        point = origin + direction * distance;
        normal = (point - body.position) / circle->radius;
        return true;
    } else if (body.shape->GetType() == POLYGON_TYPE) {
        Polygon* polygon = reinterpret_cast<Polygon*>(body.shape);

        distance = CONST_MAXFLOAT;
        point = origin;
        normal = direction;

        // Bounding circle
        vec2 relative = body.position - origin;
        float temp_dist = glm::dot(relative, vec2(direction.y, -direction.x));
        if (temp_dist < -polygon->radius or temp_dist > polygon->radius) {
            return false;
        }

        temp_dist = 0.0f;
        int intersections = 0;
        for (uint j = polygon->vertexCount - 1, i = 0; i < polygon->vertexCount; j = i, i++) {
            if (RayCrossesFace(origin, direction, CONST_MAXFLOAT, body.position + polygon->orientation * polygon->vertices[j], body.position + polygon->orientation * polygon->vertices[i], temp_dist)) {
                intersections++;
                if (temp_dist < distance and temp_dist <= maxDistance) {
                    distance = temp_dist;
                    normal = polygon->orientation * polygon->normals[j];
                }
            }
        }
        point = origin + direction * distance;

        return intersections > 0;
    } else {
        return false;
    }
}

Body* Collider::RayCasting(const vec2& origin, const vec2& direction, const float& maxDistance) const {
    Renderer::RenderRay(origin, direction * maxDistance);
    float minDistance = CONST_MAXFLOAT;
    Body* resBody = nullptr;
    for (auto body : bodies) {
        float distance;
        vec2 point;
        vec2 normal;
        if (RayCrossesBody(origin, direction, maxDistance, *body, distance, point, normal)) {
            if (distance < minDistance) {
                Renderer::RenderRay(point, normal * 0.3f);
                Renderer::RenderPoint(point);
                resBody = body;
            }
        }
    }
    return resBody;
}



inline bool CircleCrossesBody(const vec2& origin, const float& radius, const Body& body, float& distance, vec2& point, vec2& normal) {
    if (body.shape->GetType() == CIRCLE_TYPE) {
        Circle* circle = reinterpret_cast<Circle*>(body.shape);

        vec2 normal = body.position - origin;

        float distance = glm::length(normal); // LENSQR
        float radiusSum = radius + circle->radius;

        if (distance >= radiusSum) {
            return false;
        }
//        if (distance == 0.0f) {
//            normal = vec2(1.0f, 0.0f);
//            point = body.position;
//        } else {
//            normal = normal / distance; // Faster than using Normalized since we already performed sqrt
//            point = normal * circle->radius + body.position;
//        }
        return true;
    } else if (body.shape->GetType() == POLYGON_TYPE) {
        Polygon* polygon = reinterpret_cast<Polygon*>(body.shape);

        float distance = glm::length(body.position - origin); // LENSQR
        float radiusSum = radius + polygon->radius;
        if (distance >= radiusSum) {
            return false;
        }

        vec2 center = glm::transpose(polygon->orientation) * (origin - body.position);

        float separation = -CONST_MAXFLOAT;
        uint faceNormal = 0;
        for (uint i = 0; i < polygon->vertexCount; ++i) {
            float s = glm::dot(polygon->normals[i], center - polygon->vertices[i]);
            if (s > radius) {
                return false;
            }
            if (s > separation) {
                separation = s;
                faceNormal = i;
            }
        }

        vec2 v1 = polygon->vertices[faceNormal];
        uint i2 = faceNormal + 1 < polygon->vertexCount ? faceNormal + 1 : 0;
        vec2 v2 = polygon->vertices[i2];

//        if (separation < CONST_EPSILON) {
//            return false;
//        }

        float dot1 = glm::dot(center - v1, v2 - v1);
        float dot2 = glm::dot(center - v2, v1 - v2);

        if (dot1 <= 0.0f) {
            if (glm::distance(center, v1) > radius) // DISTSQR
                return false;
        } else if (dot2 <= 0.0f) {
            if (glm::distance(center, v2) > radius) // DISTSQR
                return false;
        } else {
            vec2 normal = polygon->normals[faceNormal];
            if (glm::dot(center - v1, normal) > radius)
                return false;
        }
        return true;
    } else {
        return false;
    }
}

void Collider::CircleCasting(const vec2& origin, const float& radius) const {
    float theta = 0.0f;
    float inc = CONST_PI / 30.0f;
    vec2 a = origin + vec2(std::cos(theta), std::sin(theta)) * radius;
    for (uint i = 1; i < 60; ++i) {
        theta += inc;
        vec2 b = origin + vec2(std::cos(theta), std::sin(theta)) * radius;
        Renderer::RenderLine(a, b);
        a = b;
    }
    Renderer::RenderLine(a, origin + vec2(radius, 0.0f));
    for (auto body : bodies) {
        float distance;
        vec2 point;
        vec2 normal;
        if (CircleCrossesBody(origin, radius, *body, distance, point, normal)) {
            Renderer::RenderLine(origin, body->position);
//            Renderer::RenderRay(point, normal * 0.3f);
//            Renderer::RenderPoint(point);
        }
    }
}
//}
