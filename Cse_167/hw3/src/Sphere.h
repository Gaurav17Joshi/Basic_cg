#ifndef SPHERE_H
#define SPHERE_H

#include "Hittable.h"
#include "Material.h" // Include Material
#include <glm/glm.hpp> // Core GLM library
#include <cmath> // For std::sqrt

class Sphere : public Hittable {
public:
    point3 center;
    float radius;
    Material material; // Add material

    Sphere() : center(0,0,0), radius(1.0f) {}
    Sphere(point3 cen, float r, const Material& mat) : center(cen), radius(r), material(mat) {} // Updated constructor

    virtual bool intersect(const Ray& r, float t_min, float t_max, HitRecord& rec) const override {
        glm::vec3 oc = r.origin - center;
        float a = glm::dot(r.direction, r.direction);
        float half_b = glm::dot(oc, r.direction);
        float c = glm::dot(oc, oc) - radius*radius;
        float discriminant = half_b*half_b - a*c;

        if (discriminant < 0) {
            return false; // No real roots, no intersection
        }
        
        float sqrt_d = std::sqrt(discriminant);

        // Find the nearest root that lies within the acceptable range [t_min, t_max]
        float root = (-half_b - sqrt_d) / a;
        if (root < t_min || root > t_max) {
            root = (-half_b + sqrt_d) / a;
            if (root < t_min || root > t_max) {
                return false; // No intersection in the valid range
            }
        }

        rec.t = root;
        rec.p = r.at(rec.t);
        glm::vec3 outward_normal = (rec.p - center) / radius;
        rec.set_face_normal(r, outward_normal);
        rec.material = &material; // Store pointer to material in hit record

        return true;
    }
};

#endif