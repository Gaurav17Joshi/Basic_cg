#ifndef TRANSFORMED_HITTABLE_H
#define TRANSFORMED_HITTABLE_H

#include "Hittable.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// A wrapper class that applies a transformation matrix to any Hittable object.
class TransformedHittable : public Hittable {
public:
    Hittable* object;
    glm::mat4 object_to_world;
    glm::mat4 world_to_object;
    glm::mat4 normal_transform;

    TransformedHittable(Hittable* obj, const glm::mat4& transform_matrix)
        : object(obj), object_to_world(transform_matrix) {
        world_to_object = glm::inverse(object_to_world);
        normal_transform = glm::transpose(world_to_object);
    }

    virtual ~TransformedHittable() {
        delete object;
    }

    virtual bool intersect(const Ray& r_in, float t_min, float t_max, HitRecord& rec) const override {
        // Transform ray from world space to object space
        point3 origin_obj = glm::vec3(world_to_object * glm::vec4(r_in.origin, 1.0f));
        glm::vec3 direction_obj = glm::vec3(world_to_object * glm::vec4(r_in.direction, 0.0f));
        Ray r_obj(origin_obj, direction_obj);

        // Intersect with the wrapped object in its local space
        if (!object->intersect(r_obj, t_min, t_max, rec)) {
            return false;
        }

        // Transform intersection results from object space back to world space
        rec.p = glm::vec3(object_to_world * glm::vec4(rec.p, 1.0f));
        rec.normal = glm::normalize(glm::vec3(normal_transform * glm::vec4(rec.normal, 0.0f)));
        rec.set_face_normal(r_in, rec.normal);
        
        return true;
    }
};

#endif