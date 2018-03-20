#include "../Precompiled.h"

Circle::Circle(float radius) {
    this->radius = radius;
}

Shape* Circle::Clone() const {
    return new Circle(radius);
}

void Circle::Initialize() {
    ComputeMass(1.0f);
}

void Circle::ComputeMass(float density) {
    body->mass = CONST_PI * radius * radius * density;
    body->invMass = (body->mass) ? 1.0f / body->mass : 0.0f;
    body->moment = body->mass * radius * radius;
    body->invMoment = (body->moment) ? 1.0f / body->moment : 0.0f;
}

void Circle::SetOrientation(float radians) {}

void Circle::Draw() const {
    // Render a circle with a bunch of lines
    float theta = body->orientation;
    float inc = CONST_PI / 30.0f;
    vec2 a = body->position + vec2(std::cos(theta), std::sin(theta)) * radius;
    vec2 b;
    for (uint i = 1; i < 60; ++i) {
        theta += inc;
        b = body->position + vec2(std::cos(theta), std::sin(theta)) * radius;
        Renderer::RenderLine(a, b);
        a = b;
    }
    Renderer::RenderLine(a, body->position + vec2(std::cos(body->orientation), std::sin(body->orientation)) * radius);

    // Render line from center to edge to see rotation
    Renderer::RenderRay(body->position, vec2(std::sin(body->orientation), std::cos(body->orientation)) * radius);
}

ShapeType Circle::GetType() const {
    return CIRCLE_TYPE;
}

Circle::~Circle() {

}



Polygon::Polygon(vec2* vertices, uint count) {
    // No hulls with less than 3 vertices (ensure actual polygon)
    assert(count > 2 and count <= MaxPolyVertexCount);
    count = std::min((int)count, MaxPolyVertexCount);

    this->radius = 0.0f;

    // Find the right most point on the hull
    int rightMost = 0;
    float highestXCoord = vertices[0].x;
    for (uint i = 1; i < count; ++i) {
        float x = vertices[i].x;
        if (x > highestXCoord) {
            highestXCoord = x;
            rightMost = i;
        }

        // If matching x then take farthest negative y
        else if (x == highestXCoord)
            if (vertices[i].y < vertices[rightMost].y)
                rightMost = i;
    }

    int hull[MaxPolyVertexCount];
    int outCount = 0;
    int indexHull = rightMost;

    while (true) {
        hull[outCount] = indexHull;

        // Search for next index that wraps around the hull
        // by computing cross products to find the most counter-clockwise
        // vertex in the set, given the previos hull index
        int nextHullIndex = 0;
        for (int i = 1; i < (int)count; ++i) {
            // Skip if same coordinate as we need three unique
            // points in the set to perform a cross product
            if (nextHullIndex == indexHull) {
                nextHullIndex = i;
                continue;
            }

            // Cross every set of three unique vertices
            // Record each counter clockwise third vertex and add to the output hull
            // See : http://www.oocities.org/pcgpe/math2d.html
            vec2 e1 = vertices[nextHullIndex] - vertices[hull[outCount]];
            vec2 e2 = vertices[i] - vertices[hull[outCount]];
            float c = cross(e1, e2);
            if (c < 0.0f)
                nextHullIndex = i;

            // Cross product is zero then e vectors are on same line
            // therefor want to record vertex farthest along that line
            if (c == 0.0f and glm::length(e2) > glm::length(e1)) // LENSQR
                nextHullIndex = i;
        }

        ++outCount;
        indexHull = nextHullIndex;

        // Conclude algorithm upon wrap-around
        if (nextHullIndex == rightMost) {
            vertexCount = outCount;
            break;
        }
    }

    // Copy vertices into shape's vertices
    for (uint i = 0; i < vertexCount; ++i) {
        this->vertices[i] = vertices[hull[i]];
    }

    // Compute face normals
    for (uint i = 0; i < vertexCount; ++i) {
        uint j = i + 1 < vertexCount ? i + 1 : 0;
        vec2 face = this->vertices[j] - this->vertices[i];

        // Ensure no zero-length edges, because that's bad
        assert(glm::length(face) > CONST_EPSILON); // LENSQR

        // Calculate normal with 2D cross product between vector and scalar
        normals[i] = vec2(face.y, -face.x);
        normals[i] = glm::normalize(normals[i]);
    }
}

Polygon::Polygon(float width, float height) {
    vertexCount = 4;
    vertices[0] = vec2(-width, -height);
    vertices[1] = vec2( width, -height);
    vertices[2] = vec2( width,  height);
    vertices[3] = vec2(-width,  height);
    normals[0] = vec2( 0.0f, -1.0f);
    normals[1] = vec2( 1.0f,  0.0f);
    normals[2] = vec2( 0.0f,  1.0f);
    normals[3] = vec2(-1.0f,  0.0f);
    radius = sqrt(width * width + height * height);
}

Shape* Polygon::Clone() const {
    Polygon* polygon = new Polygon(0.0f, 0.0f);
    polygon->vertexCount = this->vertexCount;
    polygon->orientation = this->orientation;
    for (uint i = 0; i < this->vertexCount; ++i) {
        polygon->vertices[i] = this->vertices[i];
        polygon->normals[i] = this->normals[i];
    }
    polygon->radius = this->radius;
    return polygon;
}

void Polygon::Initialize() {
    ComputeMass(1.0f);
}

void Polygon::ComputeMass(float density) {
    // Calculate centroid and moment of interia
    vec2 centroid = vec2(0.0f, 0.0f); // centroid
    float area = 0.0f;
    float moment = 0.0f;
    const float k_inv3 = 1.0f / 3.0f;

    for (uint i = 0; i < vertexCount; ++i) {
        // Triangle vertices, third vertex implied as (0, 0)
        vec2 p1(vertices[i]);
        uint j = i + 1 < vertexCount ? i + 1 : 0;
        vec2 p2(vertices[j]);

        float D = cross(p1, p2);
        float triangleArea = 0.5f * D;

        area += triangleArea;

        // Use area to weight the centroid average, not just vertex position
        centroid += triangleArea * k_inv3 * (p1 + p2);

        float intx2 = p1.x * p1.x + p2.x * p1.x + p2.x * p2.x;
        float inty2 = p1.y * p1.y + p2.y * p1.y + p2.y * p2.y;
        moment += (0.25f * k_inv3 * D) * (intx2 + inty2);
    }

    centroid *= 1.0f / area;

    // Translate vertices to centroid (make the centroid (0, 0)
    // for the polygon in model space)
    // Not really necessary, but I like doing this anyway
    for (uint i = 0; i < vertexCount; ++i)
        vertices[i] -= centroid;

    body->mass = density * area;
    body->invMass = (body->mass) ? 1.0f / body->mass : 0.0f;
    body->moment = moment * density;
    body->invMoment = body->moment ? 1.0f / body->moment : 0.0f;

    for (uint i = 0; i < vertexCount; ++i) {
        float vertexDistance = sqrt(vertices[i].x * vertices[i].x + vertices[i].y * vertices[i].y);
        if (vertexDistance > radius) {
            this->radius = vertexDistance;
        }
    }
}

void Polygon::SetOrientation(float radians) {
    orientation = set(radians);
}

void Polygon::Draw() const {
    vec2 a = body->position + orientation * vertices[0];
    vec2 b;
    for (uint i = 1; i < vertexCount; ++i) {
        b = body->position + orientation * vertices[i];
        Renderer::RenderLine(a, b);
        a = b;
    }
    Renderer::RenderLine(a, body->position + orientation * vertices[0]);

//    // Render a circle with a bunch of lines
//    float theta = body->orientation;
//    float inc = CONST_PI / 30.0f;
//    a = body->position + vec2(std::cos(theta), std::sin(theta)) * radius;
//    for (uint i = 1; i < 60; ++i) {
//        theta += inc;
//        b = body->position + vec2(std::cos(theta), std::sin(theta)) * radius;
//        Renderer::RenderLine(a, b);
//        a = b;
//    }
//    Renderer::RenderLine(a, body->position + vec2(std::cos(body->orientation), std::sin(body->orientation)) * radius);
}

ShapeType Polygon::GetType() const {
    return POLYGON_TYPE;
}

Polygon::~Polygon() {
    delete[] vertices;
    delete[] normals;
}

vec2 Polygon::GetSupport(vec2 dir) const { // The extreme point along a direction within a polygon
    float bestProjection = -CONST_MAXFLOAT;
    vec2 bestVertex;
    for (uint i = 0; i < vertexCount; ++i) {
        vec2 v = vertices[i];
        float projection = glm::dot(v, dir);

        if (projection > bestProjection) {
            bestVertex = v;
            bestProjection = projection;
        }
    }
    return bestVertex;
}
