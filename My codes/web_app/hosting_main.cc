#include <iostream>
#include <string>
#include <vector>
#include <fstream>

#include "book_headers/rtweekend.h"
#include "book_headers/color.h"
#include "book_headers/hittable_list.h"
#include "book_headers/sphere.h"
#include "book_headers/material.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

// =========================================================================================
// Camera and Rendering Globals
// =========================================================================================

double aspect_ratio;
int    image_width;
int    samples_per_pixel;
int    max_depth;

point3 lookfrom;
point3 lookat;
vec3   vup;
double vfov;
double defocus_angle;
double focus_dist;

int    image_height;
point3 pixel00_loc;
vec3   pixel_delta_u;
vec3   pixel_delta_v;
vec3   u, v, w;
vec3   defocus_disk_u;
vec3   defocus_disk_v;


color ray_color(const ray& r, const hittable& world, int depth) {
    hit_record rec;
    if (depth <= 0) return color(0,0,0);

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

point3 defocus_disk_sample() {
    auto p = random_in_unit_disk();
    return lookfrom + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
}

ray get_ray(int i, int j) {
    auto pixel_center = pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v);
    auto pixel_sample = pixel_center + pixel_sample_square();

    auto ray_origin = (defocus_angle <= 0) ? lookfrom : defocus_disk_sample();
    auto ray_direction = pixel_sample - ray_origin;

    return ray(ray_origin, ray_direction);
}

void initialize_camera() {
    image_height = static_cast<int>(image_width / aspect_ratio);
    image_height = (image_height < 1) ? 1 : image_height;

    auto theta = degrees_to_radians(vfov);
    auto h = tan(theta/2);
    auto viewport_height = 2 * h * focus_dist;
    auto viewport_width = viewport_height * (static_cast<double>(image_width)/image_height);

    w = unit_vector(lookfrom - lookat);
    u = unit_vector(cross(vup, w));
    v = cross(w, u);

    vec3 viewport_u = viewport_width * u;
    vec3 viewport_v = viewport_height * -v;

    pixel_delta_u = viewport_u / image_width;
    pixel_delta_v = viewport_v / image_height;

    auto viewport_upper_left = lookfrom - (focus_dist * w) - viewport_u/2 - viewport_v/2;
    pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

    auto defocus_radius = focus_dist * tan(degrees_to_radians(defocus_angle / 2));
    defocus_disk_u = u * defocus_radius;
    defocus_disk_v = v * defocus_radius;
}

// =========================================================================================

double string_to_double(const char* s) { return std::stod(s); }
int string_to_int(const char* s) { return std::stoi(s); }

void write_status_file(int current_sample, int total_samples) {
    std::ofstream status_file("render_status.json");
    if (status_file.is_open()) {
        status_file << "{\"status\": \"rendering\", \"current_sample\": " << current_sample
                    << ", \"total_samples\": " << total_samples << "}";
        status_file.close();
    }
}


int main(int argc, char* argv[]) {
    // Default Scene and Camera Configuration
    aspect_ratio      = 16.0 / 9.0;
    image_width       = 1200;
    samples_per_pixel = 10;
    max_depth         = 10;
    vfov              = 20;
    lookfrom          = point3(13,2,3);
    lookat            = point3(0,0,0);
    vup               = vec3(0,1,0);
    defocus_angle     = 0.2;
    focus_dist        = 15.0;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--cam_x" && i + 1 < argc) lookfrom.e[0] = string_to_double(argv[++i]);
        else if (arg == "--cam_y" && i + 1 < argc) lookfrom.e[1] = string_to_double(argv[++i]);
        else if (arg == "--cam_z" && i + 1 < argc) lookfrom.e[2] = string_to_double(argv[++i]);
        else if (arg == "--vfov" && i + 1 < argc) vfov = string_to_double(argv[++i]);
        else if (arg == "--samples" && i + 1 < argc) samples_per_pixel = string_to_int(argv[++i]);
        else if (arg == "--depth" && i + 1 < argc) max_depth = string_to_int(argv[++i]);
        else if (arg == "--width" && i + 1 < argc) image_width = string_to_int(argv[++i]);
        else if (arg == "--aspect" && i + 1 < argc) aspect_ratio = string_to_double(argv[++i]);
    }

    hittable_list world;
    auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5));
    world.add(make_shared<sphere>(point3(0,-1000,0), 1000, ground_material));
    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = random_double();
            point3 center(a + 0.9*random_double(), 0.2, b + 0.9*random_double());
            if ((center - point3(4, 0.2, 0)).length() > 0.9) {
                shared_ptr<material> sphere_material;
                if (choose_mat < 0.8) {
                    auto albedo = color::random() * color::random();
                    sphere_material = make_shared<lambertian>(albedo);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                } else if (choose_mat < 0.95) {
                    auto albedo = color::random(0.5, 1);
                    auto fuzz = random_double(0, 0.5);
                    sphere_material = make_shared<metal>(albedo, fuzz);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                } else {
                    sphere_material = make_shared<dielectric>(1.5);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }
    auto material1 = make_shared<dielectric>(1.5);
    world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));
    auto material2 = make_shared<lambertian>(color(0.4, 0.2, 0.1));
    world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));
    auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
    world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

    initialize_camera();

    std::vector<color> pixel_colors(image_width * image_height);
    std::vector<unsigned char> image_data(image_width * image_height * 3);

    for (int s = 1; s <= samples_per_pixel; ++s) {
        write_status_file(s, samples_per_pixel);
        for (int j = 0; j < image_height; ++j) {
            for (int i = 0; i < image_width; ++i) {
                ray r = get_ray(i, j);
                pixel_colors[j*image_width + i] += ray_color(r, world, max_depth);
            }
        }
        for (int j = 0; j < image_height; ++j) {
            for (int i = 0; i < image_width; ++i) {
                color pixel_color = pixel_colors[j*image_width + i];
                auto r = pixel_color.x();
                auto g = pixel_color.y();
                auto b = pixel_color.z();
                auto scale = 1.0 / s;
                r *= scale; g *= scale; b *= scale;
                r = sqrt(r); g = sqrt(g); b = sqrt(b);
                int index = (j * image_width + i) * 3;
                static const interval intensity(0.000, 0.999);
                image_data[index + 0] = static_cast<unsigned char>(256 * intensity.clamp(r));
                image_data[index + 1] = static_cast<unsigned char>(256 * intensity.clamp(g));
                image_data[index + 2] = static_cast<unsigned char>(256 * intensity.clamp(b));
            }
        }
        stbi_write_png("output.png", image_width, image_height, 3, image_data.data(), image_width * 3);
    }
    
    std::ofstream complete_signal("render_complete.tmp");
    if (complete_signal.is_open()) {
        complete_signal << "completed";
        complete_signal.close();
    }

    return 0;
}
