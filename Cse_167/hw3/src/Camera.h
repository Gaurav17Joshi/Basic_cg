#ifndef CAMERA_H
#define CAMERA_H

#include "Ray.h" // Includes GLM types for point3
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> // For lookAt, perspective
#include <glm/gtc/constants.hpp> // For glm::pi
#include <cmath>

class Camera {
public:
    point3 lookfrom;
    point3 lookat;
    glm::vec3 up;
    float fovy; // vertical field of view in degrees

    Camera() : lookfrom(0,0,0), lookat(0,0,-1), up(0,1,0), fovy(45) {}

    Camera(point3 lookfrom, point3 lookat, glm::vec3 up, float fovy)
        : lookfrom(lookfrom), lookat(lookat), up(up), fovy(fovy) {}

    // Generate a ray for a given pixel coordinate (u, v)
    // (u, v) are normalized coordinates from 0 to 1
    Ray get_ray(float u, float v, float aspect_ratio) const {
        // Based on the camera parameters, compute the coordinate frame
        // glm::vec3 w = glm::normalize(lookfrom - lookat);
        // glm::vec3 u_cam = glm::normalize(glm::cross(up, w));
        // glm::vec3 v_cam = glm::cross(w, u_cam);
        // The above is the traditional way. Below is a more direct approach using GLM lookAt.

        // Calculate the view basis vectors (right, up, forward)
        glm::vec3 w = glm::normalize(lookfrom - lookat); // Z-axis of camera (forward)
        glm::vec3 u_cam = glm::normalize(glm::cross(up, w)); // X-axis of camera (right)
        glm::vec3 v_cam = glm::cross(w, u_cam); // Y-axis of camera (up) - ensure it's orthogonal

        // Convert fov from degrees to radians for tan
        auto theta = fovy * glm::pi<float>() / 180.0f;
        auto half_height = glm::tan(theta / 2.0f);
        auto half_width = aspect_ratio * half_height;

        // Position of the viewport (at distance 1 from camera)
        point3 lower_left_corner = lookfrom - half_width * u_cam - half_height * v_cam - w;

        // Corrected ray generation:
        return Ray(lookfrom, glm::normalize(lower_left_corner + u * (2 * half_width * u_cam) + v * (2 * half_height * v_cam) - lookfrom));
    }
};

#endif
