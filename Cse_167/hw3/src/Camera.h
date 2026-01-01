#ifndef CAMERA_H
#define CAMERA_H

#include "Ray.h"
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <cmath>

// Defines the virtual camera for the scene, including its position, orientation, and field of view.
class Camera {
public:
    point3 lookfrom;
    point3 lookat;
    glm::vec3 up;
    float fovy;

    Camera() : lookfrom(0,0,0), lookat(0,0,-1), up(0,1,0), fovy(45) {}
    Camera(point3 lookfrom, point3 lookat, glm::vec3 up, float fovy)
        : lookfrom(lookfrom), lookat(lookat), up(up), fovy(fovy) {}

    Ray get_ray(float u, float v, float aspect_ratio) const {
        glm::vec3 w = glm::normalize(lookfrom - lookat);
        glm::vec3 u_cam = glm::normalize(glm::cross(up, w));
        glm::vec3 v_cam = glm::cross(w, u_cam);

        auto theta = fovy * glm::pi<float>() / 180.0f;
        auto half_height = glm::tan(theta / 2.0f);
        auto half_width = aspect_ratio * half_height;
        glm::vec3 horizontal = 2.0f * half_width * u_cam;
        glm::vec3 vertical = 2.0f * half_height * v_cam;

        point3 lower_left_corner = lookfrom - (horizontal / 2.0f) - (vertical / 2.0f) - w;

        glm::vec3 direction = lower_left_corner + u * horizontal + v * vertical - lookfrom;
        return Ray(lookfrom, glm::normalize(direction));
    }
};

#endif
