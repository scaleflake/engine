#ifndef MATH_H
#define MATH_H

// STL
#include <cmath>
using std::abs;
using std::sqrt;
#include <algorithm>
using std::max;
using std::min;

// GLM
#define GLM_FORCE_INLINE
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
//using glm::vec2;
typedef glm::vec2 vec2;
using glm::vec3;
using glm::vec4;
using glm::normalize;
using glm::dot;
using glm::reflect;
using glm::distance;
using glm::mat2;
using glm::mat4;
using glm::scale;

#define CONST_MAXFLOAT      0x7f800000
#define CONST_MAXDOUBLE     0x7ff0000000000000
#define CONST_MAXSHORT      65536
#define CONST_MAXINT        4294967295
#define CONST_MAXLONG       18446744073709551616

#define CONST_PI            3.14159265f
#define CONST_E             2.718281828f
#define CONST_SQRT2

#define CONST_EPSILON       0.0001f
#define CONST_SLOP          0.001f
#define CONST_CORRECTION    0.8f
#define CONST_DELTATIME     0.01666666667f

#define GRAVITY1
#define aaa
#define aab
#define aac
#define aba
#define abb
#define abc
#define aca

#ifdef GRAVITY
const vec2 gravity(0.0f, -9.0f);
#endif // GRAVITY

inline vec2 cross(vec2 const& v, float const& a) {
    return vec2(a * v.y, -a * v.x);
}
inline vec2 cross(float const& a, vec2 const& v) {
    return vec2(-a * v.y, a* v.x);
}
inline float cross(vec2 const& a, vec2 const& b) {
    return a.x * b.y - a.y * b.x;
}

inline mat2 set(float const& radians) {
    float c = std::cos(radians);
    float s = std::sin(radians);
    mat2 matrix;
    matrix[0][0] = c;
    matrix[0][1] = s;
    matrix[1][0] = -s;
    matrix[1][1] = c;
    return matrix;
}

inline vec2 Normalize(vec2 a) {
    float len = glm::length(a);
    if (len > CONST_EPSILON) {
        vec2 res;
        res = a / len;
        return res;
    } else {
        return a;
    }
}

inline float Random(float l, float h) {
    return (h - l) * (float)rand() / RAND_MAX + l;
}

inline bool Equal(float a, float b) {
    // Comparison with tolerance of EPSILON
    // '<=' instead of '<' for NaN-comparison safety
    return std::abs(a - b) <= CONST_EPSILON;
}

inline bool BiasGreaterThan(float a, float b) {
    //              relative    absolute
    return a >= b * 0.95f + a * 0.01f;
}

#endif // MATH_H
