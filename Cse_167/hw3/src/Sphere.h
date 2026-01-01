#ifndef SPHERE_H
#define SPHERE_H

#include "Hittable.h"
#include "Material.h"
#include <glm/glm.hpp>
#include <cmath>

// A sphere primitive, defined by a center point and a radius.
class Sphere : public Hittable {
public:
    point3 center;
    float radius;
    Material material;

    Sphere(point3 cen, float r, const Material& mat) : center(cen), radius(r), material(mat) {}

    virtual bool intersect(const Ray& r, float t_min, float t_max, HitRecord& rec) const override {
        glm::vec3 oc = r.origin - center;
        float a = glm::dot(r.direction, r.direction);
        float half_b = glm::dot(oc, r.direction);
        float c = glm::dot(oc, oc) - radius*radius;
        float discriminant = half_b*half_b - a*c;

        if (discriminant < 0) {
            return false;
        }
        
        float sqrt_d = std::sqrt(discriminant);

        // Find the nearest root that lies within the acceptable range.
        float root = (-half_b - sqrt_d) / a;
        if (root < t_min || root > t_max) {
            root = (-half_b + sqrt_d) / a;
            if (root < t_min || root > t_max) {
                return false;
            }
        }

        rec.t = root;
        rec.p = r.at(rec.t);
        glm::vec3 outward_normal = (rec.p - center) / radius;
        rec.set_face_normal(r, outward_normal);
        rec.material = &material;

        return true;
    }
};

#endif