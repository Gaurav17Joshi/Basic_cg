#ifndef RAY_H
#define RAY_H

#include <glm/glm.hpp> // Core GLM library

// Type aliases for glm::vec3
using point3 = glm::vec3;
using color3 = glm::vec3;

// Epsilon for floating point comparisons and shadow ray offsets
const float EPSILON = 1e-4f;

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
