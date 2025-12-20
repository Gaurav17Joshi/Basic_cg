#include <iostream>
#include <string>
#include <vector>

// Include all necessary headers from the book_code directory
#include "../book_code/rtweekend.h"
#include "../book_code/color.h"
#include "../book_code/hittable_list.h"
#include "../book_code/sphere.h"
#include "../book_code/material.h"

// STB Image Write for PNG output
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"


// --------- From camera.h ---------
// All camera logic is now self-contained in this file
// to avoid linker issues.

int    image_width       = 400;
double aspect_ratio      = 16.0 / 9.0;
int    samples_per_pixel = 100;
int    max_depth         = 50;

point3 camera_center     = point3(0, 0, 0);

// Private camera variables
int    image_height;
point3 pixel00_loc;
vec3   pixel_delta_u;
vec3   pixel_delta_v;


color ray_color(const ray& r, const hittable& world, int depth) {
    hit_record rec;

    if (depth <= 0)
        return color(0,0,0);

    if (world.hit(r, interval(0.001, infinity), rec)) {
        ray scattered;
        color attenuation;
        if (rec.mat->scatter(r, rec, attenuation, scattered))
            return attenuation * ray_color(scattered, world, depth-1);
        return color(0,0,0);
    }

    vec3 unit_direction = unit_vector(r.direction());
    auto a = 0.5 * (unit_direction.y() + 1.0);
    return (1.0-a)*color(1.0, 1.0, 1.0) + a*color(0.5, 0.7, 1.0);
}

vec3 pixel_sample_square() {
    auto px = -0.5 + random_double();
    auto py = -0.5 + random_double();
    return (px * pixel_delta_u) + (py * pixel_delta_v);
}

ray get_ray(int i, int j) {
    auto pixel_center = pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v);
    auto pixel_sample = pixel_center + pixel_sample_square();

    auto ray_origin = camera_center;
    auto ray_direction = pixel_sample - ray_origin;

    return ray(ray_origin, ray_direction);
}

void initialize_camera() {
    image_height = static_cast<int>(image_width / aspect_ratio);
    image_height = (image_height < 1) ? 1 : image_height;

    auto focal_length = 1.0;
    auto viewport_height = 2.0;
    auto viewport_width = viewport_height * (static_cast<double>(image_width)/image_height);

    auto viewport_u = vec3(viewport_width, 0, 0);
    auto viewport_v = vec3(0, -viewport_height, 0);

    pixel_delta_u = viewport_u / image_width;
    pixel_delta_v = viewport_v / image_height;

    auto viewport_upper_left = camera_center - vec3(0, 0, focal_length) - viewport_u/2 - viewport_v/2;
    pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);
}
// --------- End of Camera Logic ---------


double string_to_double(const char* s) {
    return std::stod(s);
}

int main(int argc, char* argv[]) {
    // Parse command-line arguments for camera position
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "--cam_x" && i + 1 < argc) {
            camera_center.e[0] = string_to_double(argv[i + 1]);
        } else if (std::string(argv[i]) == "--cam_y" && i + 1 < argc) {
            camera_center.e[1] = string_to_double(argv[i + 1]);
        } else if (std::string(argv[i]) == "--cam_z" && i + 1 < argc) {
            camera_center.e[2] = string_to_double(argv[i + 1]);
        }
    }

    // World
    hittable_list world;
    auto material_ground = make_shared<lambertian>(color(0.8, 0.8, 0.0));
    auto material_center = make_shared<lambertian>(color(0.1, 0.2, 0.5));
    auto material_left   = make_shared<metal>(color(0.8, 0.8, 0.8), 0.3);
    auto material_right  = make_shared<metal>(color(0.8, 0.6, 0.2), 1.0);

    world.add(make_shared<sphere>(point3( 0.0, -100.5, -1.0), 100.0, material_ground));
    world.add(make_shared<sphere>(point3( 0.0,    0.0, -1.0),   0.5, material_center));
    world.add(make_shared<sphere>(point3(-1.0,    0.0, -1.0),   0.5, material_left));
    world.add(make_shared<sphere>(point3( 1.0,    0.0, -1.0),   0.5, material_right));

    // Render
    initialize_camera();

    std::vector<unsigned char> image_data(image_width * image_height * 3);

    for (int j = 0; j < image_height; ++j) {
        std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;
        for (int i = 0; i < image_width; ++i) {
            color pixel_color(0,0,0);
            for (int s = 0; s < samples_per_pixel; ++s) {
                ray r = get_ray(i, j);
                pixel_color += ray_color(r, world, max_depth);
            }
            
            // Write color to buffer
            auto r = pixel_color.x();
            auto g = pixel_color.y();
            auto b = pixel_color.z();

            auto scale = 1.0 / samples_per_pixel;
            r *= scale;
            g *= scale;
            b *= scale;

            r = sqrt(r);
            g = sqrt(g);
            b = sqrt(b);
            
            int index = (j * image_width + i) * 3;
            static const interval intensity(0.000, 0.999);
            image_data[index + 0] = static_cast<unsigned char>(256 * intensity.clamp(r));
            image_data[index + 1] = static_cast<unsigned char>(256 * intensity.clamp(g));
            image_data[index + 2] = static_cast<unsigned char>(256 * intensity.clamp(b));
        }
    }
    
    stbi_write_png("output.png", image_width, image_height, 3, image_data.data(), image_width * 3);
    std::clog << "\rDone.                 \n";
    
    return 0;
}