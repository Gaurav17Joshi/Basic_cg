#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include <string>
#include <stack> // For transformation stack
#include "Camera.h"
#include "Hittable.h"
#include <glm/glm.hpp> // Core GLM library
#include <glm/gtc/matrix_transform.hpp> // For glm::mat4
#include "Material.h" // Include Material
#include "Light.h"    // Include Light

class Scene {
public:
    int width, height;
    std::string output_filename;
    int max_depth;
    Camera camera;
    std::vector<point3> vertices; // Store vertices for triangle definitions
    std::vector<Hittable*> objects; // Store pointers to all objects in the scene
    std::stack<glm::mat4> transform_stack; // Transformation matrix stack using GLM's mat4

    std::vector<Light*> lights; // Store pointers to all lights in the scene
    color3 global_ambient; // Global ambient color (default 0.2, 0.2, 0.2)
    Material current_material; // Material properties for newly created objects
    
    // Default attenuation values for point lights
    float att_const_default, att_linear_default, att_quadratic_default;

    Scene() : width(640), height(480), output_filename("raytrace.png"), max_depth(5),
              global_ambient(0.2, 0.2, 0.2), // Default global ambient
              att_const_default(1.0), att_linear_default(0.0), att_quadratic_default(0.0) {
        transform_stack.push(glm::mat4(1.0f)); // Initialize with identity matrix using GLM
    }

    // Destructor to free allocated objects and lights
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
