#ifndef RAY_H
#define RAY_H

#include <glm/glm.hpp>

// Type aliases for GLM vector types
using point3 = glm::vec3;
using color3 = glm::vec3;

// Epsilon for floating point comparisons and shadow ray offsets
const float EPSILON = 1e-4f;

// Represents a 3D ray with an origin point and a direction vector.
class Ray {
public:
    point3 origin;
    glm::vec3 direction;

    Ray() {}
    Ray(const point3& origin, const glm::vec3& direction)
        : origin(origin), direction(direction)
    {}

    point3 at(float t) const {
        return origin + t * direction;
    }
};

#endif
