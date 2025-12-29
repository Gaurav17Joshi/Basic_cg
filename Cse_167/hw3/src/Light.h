#ifndef LIGHT_H
#define LIGHT_H

#include "Ray.h" // Needed for point3, color3 and glm::vec3
#include <glm/glm.hpp> // Core GLM library
#include <limits> // For std::numeric_limits

class Light {
public:
    color3 light_color;

    Light(color3 c) : light_color(c) {}

    // Virtual destructor for proper cleanup of derived classes
    virtual ~Light() = default;

    // Calculates the direction from a hit point to the light, and the light's intensity at that point.
    // light_dir should be normalized. light_intensity should factor in attenuation.
    // is_directional will be true for directional lights, false for point lights.
    // light_distance is used for shadow ray intersection checks.
    virtual void get_light_info(const point3& hit_point, glm::vec3& light_dir, color3& light_intensity, bool& is_directional, float& light_distance) const = 0;
};

class DirectionalLight : public Light {
public:
    glm::vec3 direction; // Normalized direction from scene to light

    DirectionalLight(glm::vec3 dir, color3 c) : Light(c), direction(glm::normalize(dir)) {}

    void get_light_info(const point3& hit_point, glm::vec3& light_dir, color3& light_intensity, bool& is_directional, float& light_distance) const override {
        light_dir = -direction; // Direction from hit point to light
        light_intensity = light_color; // No attenuation for directional lights
        is_directional = true;
        light_distance = std::numeric_limits<float>::infinity(); // Directional lights are infinitely far
    }
};

class PointLight : public Light {
public:
    point3 position;
    // Attenuation coefficients (constant, linear, quadratic)
    float att_const, att_linear, att_quadratic;

    PointLight(point3 pos, color3 c, float ac, float al, float aq)
        : Light(c), position(pos), att_const(ac), att_linear(al), att_quadratic(aq) {}

    void get_light_info(const point3& hit_point, glm::vec3& light_dir, color3& light_intensity, bool& is_directional, float& light_distance) const override {
        glm::vec3 dir_to_light_unnormalized = position - hit_point;
        light_distance = glm::length(dir_to_light_unnormalized); // Distance to light
        light_dir = glm::normalize(dir_to_light_unnormalized); // Normalize direction

        // Calculate attenuation
        float attenuation = 1.0f / (att_const + att_linear * light_distance + att_quadratic * light_distance * light_distance);
        light_intensity = light_color * attenuation; // Apply attenuation to light color

        is_directional = false;
    }
};

#endif