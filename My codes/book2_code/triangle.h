#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "rtweekend.h"
#include "hittable.h"
#include <cmath>

class triangle : public hittable {
  public:
    triangle(const point3& _Q, const vec3& _u, const vec3& _v, shared_ptr<material> m)
      : Q(_Q), u(_u), v(_v), mat(m)
    {
        set_bounding_box();
    }

    void set_bounding_box() {
        point3 v1 = Q + u;
        point3 v2 = Q + v;
        point3 min(fmin(Q.x(), fmin(v1.x(), v2.x())),
                   fmin(Q.y(), fmin(v1.y(), v2.y())),
                   fmin(Q.z(), fmin(v1.z(), v2.z())));
        point3 max(fmax(Q.x(), fmax(v1.x(), v2.x())),
                   fmax(Q.y(), fmax(v1.y(), v2.y())),
                   fmax(Q.z(), fmax(v1.z(), v2.z())));
        bbox = aabb(min, max);
    }

    aabb bounding_box() const override { return bbox; }

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
        const float EPSILON = 1e-8;
        vec3 edge1 = u;
        vec3 edge2 = v;
        vec3 h = cross(r.direction(), edge2);
        double a = dot(edge1, h);

        if (a > -EPSILON && a < EPSILON)
            return false;    // This ray is parallel to this triangle.

        double f = 1.0 / a;
        vec3 s = r.origin() - Q;
        double u_bary = f * dot(s, h);

        if (u_bary < 0.0 || u_bary > 1.0)
            return false;

        vec3 q = cross(s, edge1);
        double v_bary = f * dot(r.direction(), q);

        if (v_bary < 0.0 || u_bary + v_bary > 1.0)
            return false;

        // At this stage we can compute t to find out where the intersection point is on the ray.
        double t = f * dot(edge2, q);

        if (ray_t.contains(t)) {
            rec.t = t;
            rec.p = r.at(t);
            vec3 outward_normal = unit_vector(cross(edge1, edge2));
            rec.set_face_normal(r, outward_normal);
            rec.mat = mat;
            rec.u = u_bary;
            rec.v = v_bary;
            return true;
        }
        else // This means that there is a line intersection but not a ray intersection.
            return false;
    }

  private:
    point3 Q;
    vec3 u, v;
    shared_ptr<material> mat;
    aabb bbox;
};

#endif
