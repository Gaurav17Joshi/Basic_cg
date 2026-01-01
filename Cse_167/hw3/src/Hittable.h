#ifndef HITTABLE_H
#define HITTABLE_H

#include "Ray.h"
#include <glm/glm.hpp>
#include "Material.h"

// Holds all information related to a single ray-object intersection event.
struct HitRecord {
    point3 p;
    glm::vec3 normal;
    float t;
    bool front_face;
    const Material* material;

    // Sets the hit record's normal vector, ensuring it always points against the incident ray.
    inline void set_face_normal(const Ray& r, const glm::vec3& outward_normal) {
        front_face = glm::dot(r.direction, outward_normal) < 0;
        normal = front_face ? outward_normal : -outward_normal;
    }
};

// Abstract base class for any object in the scene that can be intersected by a ray.
class Hittable {
public:
    // Determines if a ray `r` intersects the object within the range [t_min, t_max].
    virtual bool intersect(const Ray& r, float t_min, float t_max, HitRecord& rec) const = 0;
    virtual ~Hittable() = default;
};

#endif
