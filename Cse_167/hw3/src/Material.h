#ifndef MATERIAL_H
#define MATERIAL_H

#include "Ray.h" // Includes GLM types for color3
#include <glm/glm.hpp> // Core GLM library

class Material {
public:
    color3 diffuse;
    color3 specular;
    float shininess;
    color3 emission;
    color3 ambient; // Will be set by 'ambient' command

    Material() : diffuse(0.8, 0.8, 0.8), specular(0.0, 0.0, 0.0), shininess(0.0), emission(0.0, 0.0, 0.0), ambient(0.2, 0.2, 0.2) {}
    Material(color3 d, color3 s, float sh, color3 e, color3 a)
        : diffuse(d), specular(s), shininess(sh), emission(e), ambient(a) {}
};

#endif