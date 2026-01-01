#include "Parser.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

#include "Scene.h"
#include "Sphere.h"
#include "Triangle.h"
#include "TransformedHittable.h"
#include "Light.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Helper function to parse a single line into a vector of string tokens.
std::vector<std::string> parse_line(const std::string& line) {
    std::vector<std::string> tokens;
    std::stringstream ss(line);
    std::string token;
    while (ss >> token) {
        tokens.push_back(token);
    }
    return tokens;
}

// Reads a scene file line-by-line, interpreting commands and populating the Scene object.
void Parser::parse_scene(const std::string& filename, Scene& scene) {
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
            if (scene.transform_stack.size() > 1) {
                scene.transform_stack.pop();
            } else {
                std::cerr << "Warning: Attempted to pop transform from an empty stack. Ignoring." << std::endl;
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
