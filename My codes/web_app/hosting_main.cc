#include "rtweekend.h"
#include "hittable_list.h"
#include "sphere.h"
#include "camera.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <iostream>
#include <vector>
#include <string>

// Function to convert a string to a double
double string_to_double(const char* s) {
    return std::stod(s);
}

int main(int argc, char* argv[]) {
    // Default camera position
    point3 camera_center(0, 0, 0);

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
    world.add(make_shared<sphere>(point3(0, 0, -1), 0.5));
    world.add(make_shared<sphere>(point3(0, -100.5, -1), 100));

    // Camera
    camera cam;
    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width = 400;
    cam.samples_per_pixel = 100;
    
    // Set the camera center based on command-line arguments
    cam.center = camera_center;

    // Render
    cam.render(world, "output.png");

    return 0;
}
