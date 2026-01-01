#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include <string>
#include <stack>
#include "Camera.h"
#include "Hittable.h"
#include <glm/glm.hpp>
#include "Material.h"
#include "Light.h"

// Acts as a central container for all scene data and parser state.
class Scene {
public:
    // Image and rendering settings
    int width, height;
    std::string output_filename;
    int max_depth;

    // Core scene components
    Camera camera;
    std::vector<Hittable*> objects;
    std::vector<Light*> lights;
    color3 global_ambient;
    
    // Parser state variables
    std::vector<point3> vertices;
    std::stack<glm::mat4> transform_stack;
    Material current_material;
    float att_const_default, att_linear_default, att_quadratic_default;

    Scene() : width(640), height(480), output_filename("raytrace.png"), max_depth(5),
              global_ambient(0.2, 0.2, 0.2),
              att_const_default(1.0), att_linear_default(0.0), att_quadratic_default(0.0) {
        transform_stack.push(glm::mat4(1.0f));
    }

    ~Scene() {
        for (Hittable* obj : objects) {
            delete obj;
        }
        for (Light* light : lights) {
            delete light;
        }
    }
};

#endif
