#ifndef LIGHT_H
#define LIGHT_H

#include "Ray.h"
#include <glm/glm.hpp>
#include <limits>

// Abstract base class for all light sources in the scene.
class Light {
public:
    color3 light_color;

    Light(color3 c) : light_color(c) {}
    virtual ~Light() = default;

    // For a given point, provides the light's direction, color/intensity, and distance.
    virtual void get_light_info(const point3& hit_point, glm::vec3& light_dir, color3& light_intensity, bool& is_directional, float& light_distance) const = 0;
};

// A directional light source, assumed to be infinitely far away.
class DirectionalLight : public Light {
public:
    glm::vec3 direction;

    DirectionalLight(glm::vec3 dir, color3 c) : Light(c), direction(glm::normalize(dir)) {}

    void get_light_info(const point3& hit_point, glm::vec3& light_dir, color3& light_intensity, bool& is_directional, float& light_distance) const override {
        light_dir = -direction;
        light_intensity = light_color;
        is_directional = true;
        light_distance = std::numeric_limits<float>::infinity();
    }
};

// A point light source, which has a specific position and radiates in all directions.
class PointLight : public Light {
public:
    point3 position;
    float att_const, att_linear, att_quadratic;

    PointLight(point3 pos, color3 c, float ac, float al, float aq)
        : Light(c), position(pos), att_const(ac), att_linear(al), att_quadratic(aq) {}

    void get_light_info(const point3& hit_point, glm::vec3& light_dir, color3& light_intensity, bool& is_directional, float& light_distance) const override {
        glm::vec3 dir_to_light_unnormalized = position - hit_point;
        light_distance = glm::length(dir_to_light_unnormalized);
        light_dir = glm::normalize(dir_to_light_unnormalized);

        // Calculate attenuation
        float attenuation = 1.0f / (att_const + att_linear * light_distance + att_quadratic * light_distance * light_distance);
        light_intensity = light_color * attenuation;

        is_directional = false;
    }
};

#endif