#ifndef TRANSFORMED_HITTABLE_H
#define TRANSFORMED_HITTABLE_H

#include "Hittable.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> // For glm::inverse, glm::transpose
#include <glm/gtc/type_ptr.hpp>         // For glm::value_ptr

class TransformedHittable : public Hittable {
public:
    Hittable* object;
    glm::mat4 object_to_world; // Transformation from object space to world space
    glm::mat4 world_to_object; // Transformation from world space to object space (inverse of object_to_world)
    glm::mat4 normal_transform; // Inverse transpose of object_to_world for normals

    TransformedHittable(Hittable* obj, const glm::mat4& transform_matrix)
        : object(obj), object_to_world(transform_matrix) {
        world_to_object = glm::inverse(object_to_world);
        normal_transform = glm::transpose(world_to_object); // Inverse transpose for normals
    }

    virtual bool intersect(const Ray& r_in, float t_min, float t_max, HitRecord& rec) const override {
        // Transform the ray from world space to object space
        // For point: p_obj = M_inv * p_world
        // For vector: v_obj = M_inv * v_world (w=0)
        point3 origin_obj = glm::vec3(world_to_object * glm::vec4(r_in.origin, 1.0f));
        glm::vec3 direction_obj = glm::vec3(world_to_object * glm::vec4(r_in.direction, 0.0f));
        
        Ray r_obj(origin_obj, direction_obj);

        if (!object->intersect(r_obj, t_min, t_max, rec)) {
            return false;
        }

        // Transform the hit record back from object space to world space
        // For point: p_world = M * p_obj
        // For normal: n_world = (M^-T) * n_obj
        rec.p = glm::vec3(object_to_world * glm::vec4(rec.p, 1.0f));
        rec.normal = glm::normalize(glm::vec3(normal_transform * glm::vec4(rec.normal, 0.0f))); // Transform normal
        
        // Re-set face normal after transformation, as the normal might have flipped relative to the ray
        // This is crucial to ensure consistent normal direction after matrix transformation.
        rec.set_face_normal(r_in, rec.normal);
        
        return true;
    }

    virtual ~TransformedHittable() {
        delete object; // Delete the wrapped object when TransformedHittable is deleted
    }
};

#endif