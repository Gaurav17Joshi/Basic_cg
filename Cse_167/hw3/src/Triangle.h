#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "Hittable.h"
#include "Material.h"
#include <glm/glm.hpp>
#include <cmath>

// A triangle primitive, defined by three vertices.
class Triangle : public Hittable {
public:
    point3 v0, v1, v2;
    Material material;

    Triangle(point3 p0, point3 p1, point3 p2, const Material& mat) : v0(p0), v1(p1), v2(p2), material(mat) {}

    virtual bool intersect(const Ray& r, float t_min, float t_max, HitRecord& rec) const override {
        glm::vec3 edge1 = v1 - v0;
        glm::vec3 edge2 = v2 - v0;

        glm::vec3 pvec = glm::cross(r.direction, edge2);
        float det = glm::dot(edge1, pvec);

        // If det is close to 0, the ray is parallel to the triangle plane.
        if (std::abs(det) < EPSILON) {
            return false;
        }

        float inv_det = 1.0 / det;

        glm::vec3 tvec = r.origin - v0;
        float u = glm::dot(tvec, pvec) * inv_det;
        if (u < 0.0 || u > 1.0) {
            return false;
        }

        glm::vec3 qvec = glm::cross(tvec, edge1);
        float v = glm::dot(r.direction, qvec) * inv_det;
        if (v < 0.0 || u + v > 1.0) {
            return false;
        }

        float t = glm::dot(edge2, qvec) * inv_det;

        if (t > t_min && t < t_max) {
            rec.t = t;
            rec.p = r.at(t);
            glm::vec3 outward_normal = glm::normalize(glm::cross(edge1, edge2));
            rec.set_face_normal(r, outward_normal);
            rec.material = &material;
            return true;
        }

        return false;
    }
};

#endif
