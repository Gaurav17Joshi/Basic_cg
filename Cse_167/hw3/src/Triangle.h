#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "Hittable.h"
#include "Material.h" // Include Material
#include <glm/glm.hpp> // Core GLM library
#include <cmath> // For std::abs

// EPSILON is now defined in Ray.h

class Triangle : public Hittable {
public:
    point3 v0, v1, v2; // Vertices of the triangle
    Material material; // Add material

    Triangle() {}
    Triangle(point3 p0, point3 p1, point3 p2, const Material& mat) : v0(p0), v1(p1), v2(p2), material(mat) {} // Updated constructor

    virtual bool intersect(const Ray& r, float t_min, float t_max, HitRecord& rec) const override {
        glm::vec3 edge1 = v1 - v0;
        glm::vec3 edge2 = v2 - v0;

        glm::vec3 pvec = glm::cross(r.direction, edge2); // Updated cross call
        float det = glm::dot(edge1, pvec); // Updated dot call

        // If det is close to 0, the ray is parallel to the triangle plane
        // or the triangle is degenerate.
        if (std::abs(det) < EPSILON) { // Use EPSILON from Ray.h
            return false;
        }

        float inv_det = 1.0 / det;

        glm::vec3 tvec = r.origin - v0;
        float u = glm::dot(tvec, pvec) * inv_det; // Updated dot call
        if (u < 0.0 || u > 1.0) {
            return false;
        }

        glm::vec3 qvec = glm::cross(tvec, edge1); // Updated cross call
        float v = glm::dot(r.direction, qvec) * inv_det; // Updated dot call
        if (v < 0.0 || u + v > 1.0) {
            return false;
        }

        float t = glm::dot(edge2, qvec) * inv_det; // Updated dot call

        if (t > t_min && t < t_max) {
            rec.t = t;
            rec.p = r.at(t);
            glm::vec3 outward_normal = glm::normalize(glm::cross(edge1, edge2)); // Updated cross call
            rec.set_face_normal(r, outward_normal);
            rec.material = &material; // Store pointer to material in hit record
            return true;
        }

        return false; // No intersection within the valid t range
    }
};

#endif
