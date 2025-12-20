#include "camera.h"
#include "rtweekend.h"
#include "color.h"
#include "hittable.h"
#include "material.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <iostream>
#include <vector>

void camera::render_to_file(const hittable& world, const char* filename) {
    initialize();

    std::vector<unsigned char> image_data(image_width * image_height * 3);

    for (int j = 0; j < image_height; ++j) {
        std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;
        for (int i = 0; i < image_width; ++i) {
            color pixel_color(0,0,0);
            for (int sample = 0; sample < samples_per_pixel; ++sample) {
                ray r = get_ray(i, j);
                pixel_color += ray_color(r, world, max_depth);
            }
            
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
    
    stbi_write_png(filename, image_width, image_height, 3, image_data.data(), image_width * 3);
    std::clog << "\rDone.                 \n";
}

void camera::render(const hittable& world) {
    initialize();
    std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";
    for (int j = 0; j < image_height; ++j) {
        std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;
        for (int i = 0; i < image_width; ++i) {
            color pixel_color(0,0,0);
            for (int sample = 0; sample < samples_per_pixel; ++sample) {
                ray r = get_ray(i, j);
                pixel_color += ray_color(r, world, max_depth);
            }
            write_color(std::cout, pixel_color, samples_per_pixel);
        }
    }
    std::clog << "\rDone.\n";
}

void camera::initialize() {
    image_height = static_cast<int>(image_width / aspect_ratio);
    image_height = (image_height < 1) ? 1 : image_height;

    // Determine viewport dimensions.
    auto focal_length = 1.0;
    auto viewport_height = 2.0;
    auto viewport_width = viewport_height * (static_cast<double>(image_width)/image_height);

    // Calculate the vectors across the horizontal and down the vertical viewport edges.
    auto viewport_u = vec3(viewport_width, 0, 0);
    auto viewport_v = vec3(0, -viewport_height, 0);

    // Calculate the horizontal and vertical delta vectors from pixel to pixel.
    pixel_delta_u = viewport_u / image_width;
    pixel_delta_v = viewport_v / image_height;

    // Calculate the location of the upper left pixel.
    auto viewport_upper_left = center - vec3(0, 0, focal_length) - viewport_u/2 - viewport_v/2;
    pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);
}

ray camera::get_ray(int i, int j) const {
    auto pixel_center = pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v);
    auto pixel_sample = pixel_center + pixel_sample_square();

    auto ray_origin = center;
    auto ray_direction = pixel_sample - ray_origin;

    return ray(ray_origin, ray_direction);
}

vec3 camera::pixel_sample_square() const {
    auto px = -0.5 + random_double();
    auto py = -0.5 + random_double();
    return (px * pixel_delta_u) + (py * pixel_delta_v);
}

color camera::ray_color(const ray& r, const hittable& world, int depth) const {
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
