#ifndef HITTABLE_H
#define HITTABLE_H

#include "Ray.h" // Includes GLM types for point3
#include <glm/glm.hpp> // Core GLM library
#include "Material.h" // Include Material

// Struct to hold information about a ray-object intersection
struct HitRecord {
    point3 p;      // Intersection point
    glm::vec3 normal;   // Normal vector at the intersection point
    float t;       // Parameter t along the ray where intersection occurred
    bool front_face; // True if ray hits front face, false if back face
    const Material* material; // Pointer to the material of the hit object

    inline void set_face_normal(const Ray& r, const glm::vec3& outward_normal) {
        // Sets the hit record normal.
        // Assumes the outward_normal has unit length.

        // If the dot product is positive, the ray is inside the object,
        // so the normal should point inwards (opposite to outward_normal).
        // Otherwise, the ray is outside, and the normal points outwards.
        front_face = glm::dot(r.direction, outward_normal) < 0; // Updated dot call
        normal = front_face ? outward_normal : -outward_normal;
    }
};

// Abstract base class for any object that a ray can hit
class Hittable {
public:
    // Pure virtual function for ray-object intersection
    // Returns true if the ray hits the object, false otherwise
    // If it hits, it fills the HitRecord with intersection information
    virtual bool intersect(const Ray& r, float t_min, float t_max, HitRecord& rec) const = 0;

    // Virtual destructor to ensure proper cleanup of derived classes
    virtual ~Hittable() = default;
};

#endif
