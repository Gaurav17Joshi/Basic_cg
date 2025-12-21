// This file defines the hittable abstract base class, which represents any object
// that can be hit by a ray. It also defines the hit_record struct, which is used
// to store information about a ray-object intersection.

#ifndef HITTABLE_H
#define HITTABLE_H

#include "rtweekend.h"

class material; // Forward declaration

// Stores information about a ray-object intersection.
class hit_record {
  public:
    point3 p;
    vec3 normal;
    shared_ptr<material> mat;
    double t;
    bool front_face;

    void set_face_normal(const ray& r, const vec3& outward_normal) {
        front_face = dot(r.direction(), outward_normal) < 0;
        normal = front_face ? outward_normal : -outward_normal;
    }
};

// Abstract class for objects that can be hit by a ray.
class hittable {
  public:
    virtual ~hittable() = default;

    virtual bool hit(const ray& r, interval ray_t, hit_record& rec) const = 0;
};

#endif