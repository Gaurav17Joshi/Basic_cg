#include "Renderer.h"

#include <iostream>
#include <limits>
#include "Ray.h"
#include "Light.h"
#include "Hittable.h"
#include <glm/glm.hpp>
#include <algorithm>

// Applies the Blinn-Phong shading model for a single light source.
color3 calculate_shading(const Ray& r, const HitRecord& rec, const Scene& scene, const Light& light, const glm::vec3& light_dir, const color3& light_intensity) {
    color3 final_color(0, 0, 0);
    const Material* material = rec.material;
    const glm::vec3& N = rec.normal;
    const glm::vec3 V = glm::normalize(r.origin - rec.p);

    // Diffuse component
    float NdotL = std::max(0.0f, glm::dot(N, light_dir));
    final_color += material->diffuse * light_intensity * NdotL;

    // Specular component
    if (material->shininess > 0 && NdotL > 0) {
        glm::vec3 H = glm::normalize(light_dir + V);
        float NdotH = std::max(0.0f, glm::dot(N, H));
        final_color += material->specular * light_intensity * std::pow(NdotH, material->shininess);
    }

    return final_color;
}

// Recursively traces a single ray to determine its final color.
color3 ray_color(const Ray& r, const Scene& scene, int depth) {
    // Base case: recursion depth limit reached
    if (depth >= scene.max_depth) {
        return color3(0, 0, 0);
    }

    // Find the closest object intersection
    HitRecord rec;
    HitRecord temp_rec;
    bool hit_anything = false;
    float closest_t = std::numeric_limits<float>::max();
    for (const auto& object : scene.objects) {
        if (object->intersect(r, EPSILON, closest_t, temp_rec)) {
            hit_anything = true;
            closest_t = temp_rec.t;
            rec = temp_rec;
        }
    }

    if (hit_anything) {
        color3 final_color(0, 0, 0);
        const Material* material = rec.material;

        // Ambient and Emission
        final_color += material->ambient * scene.global_ambient;
        final_color += material->emission;

        // Lighting and Shadows
        for (const auto& light : scene.lights) {
            glm::vec3 light_dir;
            color3 light_intensity;
            bool is_directional;
            float light_distance;

            light->get_light_info(rec.p, light_dir, light_intensity, is_directional, light_distance);

            point3 shadow_ray_origin = rec.p + rec.normal * EPSILON;
            Ray shadow_ray(shadow_ray_origin, light_dir);
            HitRecord shadow_rec;
            bool in_shadow = false;
            float shadow_t_max = is_directional ? std::numeric_limits<float>::max() : light_distance;

            for (const auto& object : scene.objects) {
                if (object->intersect(shadow_ray, EPSILON, shadow_t_max, shadow_rec)) {
                    in_shadow = true;
                    break;
                }
            }

            if (!in_shadow) {
                final_color += calculate_shading(r, rec, scene, *light, light_dir, light_intensity);
            }
        }

        // Reflection
        if (glm::dot(material->specular, material->specular) > 0) {
            glm::vec3 reflected_dir = glm::reflect(glm::normalize(r.direction), rec.normal);
            point3 reflected_ray_origin = rec.p + rec.normal * EPSILON;
            Ray reflected_ray(reflected_ray_origin, reflected_dir);
            final_color += material->specular * ray_color(reflected_ray, scene, depth + 1);
        }

        return final_color;
    }

    // Background color
    glm::vec3 unit_direction = glm::normalize(r.direction);
    auto t = 0.5f * (unit_direction.y + 1.0f);
    return (1.0f - t) * color3(1.0, 1.0, 1.0) + t * color3(0.5, 0.7, 1.0);
}

// Orchestrates the rendering process for the entire scene.
BYTE* Renderer::render(const Scene& scene) {
    const int width = scene.width;
    const int height = scene.height;
    const float aspect_ratio = (float)width / (float)height;

    // Pixel buffer allocation
    BYTE* pixels = new BYTE[width * height * 3];

    std::cout << "Rendering " << width << "x" << height << " image to " << scene.output_filename << std::endl;

    // Main render loop
    for (int y = 0; y < height; ++y) {
        if (y % 10 == 0) {
            std::cerr << "\rScanlines remaining: " << height - y - 1 << ' ' << std::flush;
        }

        for (int x = 0; x < width; ++x) {
            auto u_coord = (float)(x + 0.5f) / (width);
            auto v_coord = ((float)height - (float)y - 0.5f) / (float)height;

            Ray r = scene.camera.get_ray(u_coord, v_coord, aspect_ratio);
            color3 pixel_color = ray_color(r, scene, 0);

            // Store pixel in BGR order
            int i = (y * width + x) * 3;
            pixels[i + 0] = (BYTE)(std::min(1.0f, pixel_color.b) * 255);
            pixels[i + 1] = (BYTE)(std::min(1.0f, pixel_color.g) * 255);
            pixels[i + 2] = (BYTE)(std::min(1.0f, pixel_color.r) * 255);
        }
    }

    std::cerr << "\nDone.\n";
    return pixels;
}
