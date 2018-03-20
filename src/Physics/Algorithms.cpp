#include "../Precompiled.h"

void CircleToCircle(const Body* a, const Body* b, Collision* collision);
void CircleToPolygon(const Body* a, const Body* b, Collision* collision);
void PolygonToCircle(const Body* a, const Body* b, Collision* collision);
void PolygonToPolygon(const Body* a, const Body* b, Collision* collision);

typedef void (*CollisionCallback)(const Body* a, const Body* b, Collision* collision);

CollisionCallback Dispatch[2][2] = {
    { CircleToCircle, CircleToPolygon },
    { PolygonToCircle, PolygonToPolygon }
};

void ShapeToShape(Body* a, Body* b, Collision* collision) {
    Dispatch[a->shape->GetType()][b->shape->GetType()](a, b, collision);
}

inline void CircleToCircle(const Body* a, const Body* b, Collision* collision) {
    Circle* A = reinterpret_cast<Circle*>(a->shape);
    Circle* B = reinterpret_cast<Circle*>(b->shape);

    // Calculate translational vector, which is normal
    vec2 normal = b->position - a->position;

    float distance = glm::length(normal); // LENSQR
    float radius = A->radius + B->radius;

    // Not in contact
    if (distance >= radius) {
        collision->contactCount = 0;
        return;
    }

    collision->contactCount = 1;

    if (distance == 0.0f) {
        collision->penetration = A->radius;
        collision->normal = vec2(1.0f, 0.0f);
        collision->contacts[0] = a->position;
    } else {
        collision->penetration = radius - distance;
        collision->normal = normal / distance; // Faster than using Normalized since we already performed sqrt
        collision->contacts[0] = collision->normal * A->radius + a->position;
    }
}

inline void CircleToPolygon(const Body* a, const Body* b, Collision* collision) {
    Circle* A = reinterpret_cast<Circle*>(a->shape);
    Polygon* B = reinterpret_cast<Polygon*>(b->shape);

    collision->contactCount = 0;

    // Bounding circles
    float distance = glm::length(b->position - a->position); // LENSQR
    float radius = A->radius + B->radius;
    if (distance >= radius) {
        return;
    }

    // Transform circle center to Polygon model space
    vec2 center = a->position;
    center = glm::transpose(B->orientation) * (center - b->position);

    // Find edge with minimum penetration
    // Exact concept as using support points in Polygon vs Polygon
    float separation = -CONST_MAXFLOAT;
    uint faceNormal = 0;
    for (uint i = 0; i < B->vertexCount; ++i) {
        float s = glm::dot(B->normals[i], center - B->vertices[i]);
        if (s > A->radius) {
            return;
        }
        if (s > separation) {
            separation = s;
            faceNormal = i;
        }
    }

    // Grab face's vertices
    vec2 v1 = B->vertices[faceNormal];
    uint i2 = faceNormal + 1 < B->vertexCount ? faceNormal + 1 : 0;
    vec2 v2 = B->vertices[i2];

    // Check to see if center is within polygon
    if (separation < (float)CONST_EPSILON) {
        collision->contactCount = 1;
        collision->normal = -(B->orientation * B->normals[faceNormal]);
        collision->contacts[0] = collision->normal * A->radius + a->position;
        collision->penetration = A->radius;
        return;
    }

    // Determine which voronoi region of the edge center of circle lies within
    float dot1 = glm::dot(center - v1, v2 - v1);
    float dot2 = glm::dot(center - v2, v1 - v2);
    collision->penetration = A->radius - separation;

    // Closest to v1
    if (dot1 <= 0.0f) {
        if (glm::distance(center, v1) > A->radius) // DISTSQR
            return;

        collision->contactCount = 1;
        vec2 normal = v1 - center;
        normal = B->orientation * normal;
        normal = Normalize(normal);
        collision->normal = normal;
        v1 = B->orientation * v1 + b->position;
        collision->contacts[0] = v1;
    } else if (dot2 <= 0.0f) { // Closest to v2
        if (glm::distance(center, v2) > A->radius) // DISTSQR
            return;

        collision->contactCount = 1;
        vec2 normal = v2 - center;
        v2 = B->orientation * v2 + b->position;
        collision->contacts[0] = v2;
        normal = B->orientation * normal;
        normal = Normalize(normal);
        collision->normal = normal;
    } else { // Closest to face
        vec2 normal = B->normals[faceNormal];
        if (glm::dot(center - v1, normal) > A->radius)
            return;

        normal = B->orientation * normal;
        collision->normal = -1.0f * normal;
        collision->contacts[0] = collision->normal * A->radius + a->position;
        collision->contactCount = 1;
    }
}

inline void PolygonToCircle(const Body* a, const Body* b, Collision* collision) {
    CircleToPolygon(b, a, collision);
    collision->normal *= -1.0f;
}


inline float FindAxisLeastPenetration(const Polygon* A, const Polygon* B, uint& faceIndex) {
    float bestDistance = -CONST_MAXFLOAT;
    uint bestIndex;

    for (uint i = 0; i < A->vertexCount; ++i) {
        // Retrieve a face normal from A
        vec2 n = A->normals[i];
        vec2 nw = A->orientation * n;

        // Transform face normal into B's model space
        mat2 transposedOrientation = glm::transpose(B->orientation);
        n = transposedOrientation * nw;

        // Retrieve support point from B along -n
        vec2 s = B->GetSupport(-n);

        // Retrieve vertex on face from A, transform into
        // B's model space
        vec2 v = A->vertices[i];
        v = A->orientation * v + A->body->position;
        v -= B->body->position;
        v = transposedOrientation * v;

        // Compute penetration distance (in B's model space)
        float d = glm::dot(n, s - v);

        // Store greatest distance
        if (d > bestDistance) {
            bestDistance = d;
            bestIndex = i;
        }
    }

    faceIndex = bestIndex;
    return bestDistance;
}

inline int Clip(const vec2& n, const float& c, vec2* face) {
    uint sp = 0;
    vec2 out[2] = { face[0], face[1] };

    // Retrieve distances from each endpoint to the line
    // d = ax + by - c
    float d1 = glm::dot(n, face[0]) - c;
    float d2 = glm::dot(n, face[1]) - c;

    // If negative (behind plane) clip
    if (d1 <= 0.0f)
        out[sp++] = face[0];
    if (d2 <= 0.0f)
        out[sp++] = face[1];

    // If the points are on different sides of the plane
    if (d1 * d2 < 0.0f) { // less than to ignore -0.0f
        // Push interesection point
        float alpha = d1 / (d1 - d2);

        out[sp] = face[0] + alpha * (face[1] - face[0]);
        ++sp;
    }

    // Assign our new converted values
    face[0] = out[0];
    face[1] = out[1];

    assert(sp != 3);

    return sp;
}

inline void PolygonToPolygon(const Body* a, const Body* b, Collision* collision) {
    Polygon* A = reinterpret_cast<Polygon*>(a->shape);
    Polygon* B = reinterpret_cast<Polygon*>(b->shape);

    collision->contactCount = 0;

    // Bounding circles
    float distance = glm::length(b->position - a->position); // LENSQR
    float radius = A->radius + B->radius;
    if (distance >= radius) {
        return;
    }

    // Check for a separating axis with A's face planes
    uint faceA;
    float penetrationA = FindAxisLeastPenetration(A, B, faceA);
    if (penetrationA >= 0.0f)
        return;

    // Check for a separating axis with B's face planes
    uint faceB;
    float penetrationB = FindAxisLeastPenetration(B, A, faceB);
    if (penetrationB >= 0.0f)
        return;

    uint referenceIndex;
    bool flip; // Always point from a to b

    Polygon* RefPoly; // Reference
    Polygon* IncPoly; // Incident

    // Determine which shape contains reference face
    if (BiasGreaterThan(penetrationA, penetrationB)) {
        RefPoly = A;
        IncPoly = B;
        referenceIndex = faceA;
        flip = false;
    } else {
        RefPoly = B;
        IncPoly = A;
        referenceIndex = faceB;
        flip = true;
    }

    // World space incident face
    vec2 incidentFace[2];

    // FindIncidentFace
    auto findIncidentFace = [&RefPoly, &IncPoly, &referenceIndex, &incidentFace]() -> void {
        vec2 referenceNormal = RefPoly->normals[referenceIndex];

        // Calculate normal in incident's frame of reference
        referenceNormal = RefPoly->orientation * referenceNormal; // To world space
        referenceNormal = glm::transpose(IncPoly->orientation) * referenceNormal; // To incident's model space

        // Find most anti-normal face on incident polygon
        uint incidentFaceNo = 0;
        float minDot = CONST_MAXFLOAT;
        for (uint i = 0; i < IncPoly->vertexCount; ++i) {
            float dot = glm::dot(referenceNormal, IncPoly->normals[i]);
            if (dot < minDot) {
                minDot = dot;
                incidentFaceNo = i;
            }
        }

        // Assign face vertices for incidentFace
        incidentFace[0] = IncPoly->orientation * IncPoly->vertices[incidentFaceNo] + IncPoly->body->position;
        incidentFaceNo = incidentFaceNo + 1 >= IncPoly->vertexCount ? 0 : incidentFaceNo + 1;
        incidentFace[1] = IncPoly->orientation * IncPoly->vertices[incidentFaceNo] + IncPoly->body->position;
    };

    findIncidentFace();

    // Setup reference face vertices
    vec2 v1 = RefPoly->vertices[referenceIndex];
    referenceIndex = referenceIndex + 1 == RefPoly->vertexCount ? 0 : referenceIndex + 1;
    vec2 v2 = RefPoly->vertices[referenceIndex];

    // Transform vertices to world space
    v1 = RefPoly->orientation * v1 + RefPoly->body->position;
    v2 = RefPoly->orientation * v2 + RefPoly->body->position;

    // Calculate reference face side normal in world space
    vec2 sidePlaneNormal = glm::normalize(v2 - v1);

    // Orthogonalize
    vec2 refFaceNormal(sidePlaneNormal.y, -sidePlaneNormal.x);

    // ax + by = c
    // c is distance from origin
    float refC = glm::dot(refFaceNormal, v1);
    float negSide = -1.0f * glm::dot(sidePlaneNormal, v1);
    float posSide = glm::dot(sidePlaneNormal, v2);

    // Clip incident face to reference face side planes
    if (Clip(-sidePlaneNormal, negSide, incidentFace) < 2)
        return; // Due to floating point error, possible to not have required points
    if (Clip(sidePlaneNormal, posSide, incidentFace) < 2)
        return; // Due to floating point error, possible to not have required points
    // Flip
    collision->normal = flip ? -refFaceNormal : refFaceNormal;
    // Keep points behind reference face
    uint cp = 0; // clipped points behind reference face
    float separation = glm::dot(refFaceNormal, incidentFace[0]) - refC;
    if (separation <= 0.0f) {
        collision->contacts[cp] = incidentFace[0];
        collision->penetration = -separation;
        ++cp;
    } else {
        collision->penetration = 0;
    }
    separation = glm::dot(refFaceNormal, incidentFace[1]) - refC;
    if (separation <= 0.0f) {
        collision->contacts[cp] = incidentFace[1];

        collision->penetration += -separation;
        ++cp;

        // Average penetration
        collision->penetration = collision->penetration / (float)cp;
    }
    collision->contactCount = cp;
}
