#define GLM_ENABLE_EXPERIMENTAL // Must be defined before any GLM include that uses experimental features

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <limits> // Required for numeric_limits
#include <FreeImage.h>

#include "Ray.h" // Includes GLM types for point3, color3
#include "Camera.h"
#include "Scene.h"
#include "Sphere.h"
#include "Triangle.h"
#include "TransformedHittable.h"
#include "Material.h"
#include "Light.h"

#include <glm/glm.hpp> // Core GLM library
#include <glm/gtc/matrix_transform.hpp> // For glm::translate, rotate, scale
#include <glm/gtc/type_ptr.hpp>         // For glm::value_ptr (though not directly used, good to have)
#include <glm/gtc/constants.hpp>        // For glm::pi
#include <glm/gtx/string_cast.hpp>      // For debugging glm types (optional, but requires experimental enable)

// Function to parse a line into a command and its arguments
std::vector<std::string> parse_line(const std::string& line) {
    std::vector<std::string> tokens;
    std::stringstream ss(line);
    std::string token;
    while (ss >> token) {
        tokens.push_back(token);
    }
    return tokens;
}

// Blinn-Phong shading calculation
color3 calculate_shading(const Ray& r, const HitRecord& rec, const Scene& scene, const Light& light, const glm::vec3& light_dir, const color3& light_intensity) {
    color3 final_color(0, 0, 0);

    const Material* material = rec.material;
    const glm::vec3& N = rec.normal; // Normal at hit point
    const glm::vec3 V = glm::normalize(r.origin - rec.p); // View direction from hit point to camera

    // Diffuse component
    float NdotL = std::max(0.0f, glm::dot(N, light_dir));
    final_color += material->diffuse * light_intensity * NdotL;

    // Specular component
    if (material->shininess > 0 && glm::dot(N,light_dir) > 0) { // Only add specular if light is on same side as normal
        glm::vec3 H = glm::normalize(light_dir + V); // Halfway vector
        float NdotH = std::max(0.0f, glm::dot(N, H));
        final_color += material->specular * light_intensity * std::pow(NdotH, material->shininess);
    }

    return final_color;
}


// Function to determine the color of a ray
color3 ray_color(const Ray& r, const Scene& scene, int depth) {
    // If we've exceeded the ray bounce limit, no more light is gathered.
    if (depth >= scene.max_depth) {
        return color3(0,0,0);
    }

    HitRecord rec; // This will hold the final, closest hit
    HitRecord temp_rec; // Temporary record for each object test
    bool hit_anything = false;
    float closest_t = std::numeric_limits<float>::max();

    // Iterate through all objects to find the closest intersection
    for (const auto& object : scene.objects) {
        if (object->intersect(r, EPSILON, closest_t, temp_rec)) { // Use temp_rec
            hit_anything = true;
            closest_t = temp_rec.t;
            rec = temp_rec; // Copy temp_rec to rec only when a new closest hit is found
        }
    }

    if (hit_anything) {
        color3 final_color(0,0,0);
        const Material* material = rec.material;

        // Ambient component (object's ambient * global ambient)
        final_color += material->ambient * scene.global_ambient;

        // Emission component
        final_color += material->emission;

        // Iterate over all lights for diffuse and specular
        for (const auto& light : scene.lights) {
            glm::vec3 light_dir;
            color3 light_intensity;
            bool is_directional;
            float light_distance; // Distance for point lights, infinity for directional

            light->get_light_info(rec.p, light_dir, light_intensity, is_directional, light_distance);

            // Cast shadow ray
            // Add a small epsilon offset to the hit point to avoid self-intersection
            point3 shadow_ray_origin = rec.p + rec.normal * EPSILON; 
            Ray shadow_ray(shadow_ray_origin, light_dir);
            HitRecord shadow_rec;
            bool in_shadow = false;

            float shadow_t_max = is_directional ? std::numeric_limits<float>::max() : light_distance;
            
            // Check for intersections between hit_point and light source
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
        
        // Reflection component (if material has specular properties)
        if (glm::dot(material->specular, material->specular) > 0) { // Using glm::dot for squared length
            glm::vec3 reflected_dir = glm::reflect(glm::normalize(r.direction), rec.normal);
            // Again, epsilon offset to avoid self-intersection
            point3 reflected_ray_origin = rec.p + rec.normal * EPSILON;
            Ray reflected_ray(reflected_ray_origin, reflected_dir);
            final_color += material->specular * ray_color(reflected_ray, scene, depth + 1);
        }

        return final_color;

    }

    // If no object is hit, return the background color (blue-to-white gradient)
    glm::vec3 unit_direction = glm::normalize(r.direction);
    auto t = 0.5f * (unit_direction.y + 1.0f);
    return (1.0f - t) * color3(1.0, 1.0, 1.0) + t * color3(0.5, 0.7, 1.0);
}

void parse_scene(const std::string& filename, Scene& scene) {
    std::ifstream infile(filename);
    if (!infile) {
        std::cerr << "Error: Could not open scene file: " << filename << std::endl;
        exit(1);
    }

    std::string line;
    while (std::getline(infile, line)) {
        if (line.empty() || line[0] == '#') {
            continue;
        }

        std::vector<std::string> tokens = parse_line(line);
        if (tokens.empty()) {
            continue;
        }

        std::string cmd = tokens[0];
        if (cmd == "size" && tokens.size() == 3) {
            scene.width = std::stoi(tokens[1]);
            scene.height = std::stoi(tokens[2]);
        } else if (cmd == "output" && tokens.size() == 2) {
            scene.output_filename = tokens[1];
        } else if (cmd == "maxdepth" && tokens.size() == 2) {
            scene.max_depth = std::stoi(tokens[1]);
        } else if (cmd == "camera" && tokens.size() == 11) {
            point3 lookfrom(std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3]));
            point3 lookat(std::stof(tokens[4]), std::stof(tokens[5]), std::stof(tokens[6]));
            glm::vec3 up(std::stof(tokens[7]), std::stof(tokens[8]), std::stof(tokens[9]));
            float fovy = std::stof(tokens[10]);
            scene.camera = Camera(lookfrom, lookat, up, fovy);
        } else if (cmd == "sphere" && tokens.size() == 5) {
            point3 center(std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3]));
            float radius = std::stof(tokens[4]);
            scene.objects.push_back(new TransformedHittable(new Sphere(center, radius, scene.current_material), scene.transform_stack.top()));
        } else if (cmd == "vertex" && tokens.size() == 4) {
            scene.vertices.emplace_back(std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3]));
        } else if (cmd == "tri" && tokens.size() == 4) {
            int v0_idx = std::stoi(tokens[1]);
            int v1_idx = std::stoi(tokens[2]);
            int v2_idx = std::stoi(tokens[3]);
            if (v0_idx < scene.vertices.size() && v1_idx < scene.vertices.size() && v2_idx < scene.vertices.size()) {
                scene.objects.push_back(new TransformedHittable(new Triangle(scene.vertices[v0_idx], scene.vertices[v1_idx], scene.vertices[v2_idx], scene.current_material), scene.transform_stack.top()));
            } else {
                std::cerr << "Warning: Invalid vertex index for triangle command: " << line << std::endl;
            }
        } else if (cmd == "pushTransform") {
            scene.transform_stack.push(scene.transform_stack.top());
        } else if (cmd == "popTransform") {
            if (scene.transform_stack.size() > 1) { // Always keep at least one matrix (identity)
                scene.transform_stack.pop();
            } else {
                std::cerr << "Warning: Attempted to pop transform from an empty stack (or stack with only identity). Ignoring."
 << std::endl;
            }
        } else if (cmd == "translate" && tokens.size() == 4) {
            float x = std::stof(tokens[1]);
            float y = std::stof(tokens[2]);
            float z = std::stof(tokens[3]);
            scene.transform_stack.top() = glm::translate(scene.transform_stack.top(), glm::vec3(x, y, z));
        } else if (cmd == "rotate" && tokens.size() == 5) {
            float x = std::stof(tokens[1]);
            float y = std::stof(tokens[2]);
            float z = std::stof(tokens[3]);
            float angle = std::stof(tokens[4]);
            scene.transform_stack.top() = glm::rotate(scene.transform_stack.top(), glm::radians(angle), glm::vec3(x, y, z));
        } else if (cmd == "scale" && tokens.size() == 4) {
            float x = std::stof(tokens[1]);
            float y = std::stof(tokens[2]);
            float z = std::stof(tokens[3]);
            scene.transform_stack.top() = glm::scale(scene.transform_stack.top(), glm::vec3(x, y, z));
        } else if (cmd == "directional" && tokens.size() == 7) {
            glm::vec3 dir(std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3]));
            color3 col(std::stof(tokens[4]), std::stof(tokens[5]), std::stof(tokens[6]));
            scene.lights.push_back(new DirectionalLight(dir, col));
        } else if (cmd == "point" && tokens.size() == 7) {
            point3 pos(std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3]));
            color3 col(std::stof(tokens[4]), std::stof(tokens[5]), std::stof(tokens[6]));
            scene.lights.push_back(new PointLight(pos, col, scene.att_const_default, scene.att_linear_default, scene.att_quadratic_default));
        } else if (cmd == "attenuation" && tokens.size() == 4) {
            scene.att_const_default = std::stof(tokens[1]);
            scene.att_linear_default = std::stof(tokens[2]);
            scene.att_quadratic_default = std::stof(tokens[3]);
        } else if (cmd == "ambient" && tokens.size() == 4) {
            // This command sets the ambient property of the current material
            scene.current_material.ambient = color3(std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3]));
        } else if (cmd == "diffuse" && tokens.size() == 4) {
            scene.current_material.diffuse = color3(std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3]));
        } else if (cmd == "specular" && tokens.size() == 4) {
            scene.current_material.specular = color3(std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3]));
        } else if (cmd == "shininess" && tokens.size() == 2) {
            scene.current_material.shininess = std::stof(tokens[1]);
        } else if (cmd == "emission" && tokens.size() == 4) {
            scene.current_material.emission = color3(std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3]));
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <scene_file>" << std::endl;
        return 1;
    }

    Scene scene;
    parse_scene(argv[1], scene);

    // Ensure transform stack is not empty after parsing
    if (scene.transform_stack.empty()) {
        std::cerr << "Error: Transform stack is empty after parsing. This should not happen."
 << std::endl;
        return 1;
    }


    const int width = scene.width;
    const int height = scene.height;
    const float aspect_ratio = (float)width / (float)height;

    // Allocate pixel buffer
    BYTE* pixels = new BYTE[width * height * 3];

    std::cout << "Rendering " << width << "x" << height << " image to " << scene.output_filename << std::endl;

    // Main rendering loop
    for (int y = 0; y < height; ++y) {
        // Progress indicator
        if (y % 10 == 0) { // Changed to every 10 scanlines for more frequent updates
            std::cerr << "\rScanlines remaining: " << height - y - 1 << ' ' << std::flush;
        }

        for (int x = 0; x < width; ++x) {
            // Get normalized coordinates (u,v) through the pixel center
            auto u_coord = (float)(x + 0.5f) / (width);
            // Invert v-coordinate to fix image inversion. Top of image (y=0) should map to top of viewport (v=1).
            auto v_coord = ((float)height - (float)y - 0.5f) / (float)height;

            Ray r = scene.camera.get_ray(u_coord, v_coord, aspect_ratio);
            color3 pixel_color = ray_color(r, scene, 0); // Initial depth 0

            // Convert color from float [0,1] to byte [0,255]
            // And store in BGR order for FreeImage
            int i = (y * width + x) * 3;
            pixels[i + 0] = (BYTE)(std::min(1.0f, pixel_color.b) * 255); // B
            pixels[i + 1] = (BYTE)(std::min(1.0f, pixel_color.g) * 255); // G
            pixels[i + 2] = (BYTE)(std::min(1.0f, pixel_color.r) * 255); // R
        }
    }
    std::cerr << "\nDone.\n";
    
    // Save image using FreeImage
    FreeImage_Initialise();
    // The 'true' argument indicates top-left pixel is first, consistent with typical image rendering
    // The masks are for RGB order
    FIBITMAP* img = FreeImage_ConvertFromRawBits(pixels, width, height, width * 3, 24, 0xFF0000, 0x00FF00, 0x0000FF, true);
    if (!FreeImage_Save(FIF_PNG, img, scene.output_filename.c_str(), 0)) {
        std::cerr << "Error: Could not save image " << scene.output_filename << std::endl;
    }
    FreeImage_Unload(img);
    FreeImage_DeInitialise();

    // Cleanup
    delete[] pixels;

    return 0;
}