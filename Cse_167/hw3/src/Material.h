#ifndef MATERIAL_H
#define MATERIAL_H

#include "Ray.h"
#include <glm/glm.hpp>

// Holds all surface properties for a given material (color, shininess, etc.).
class Material {
public:
    color3 diffuse;
    color3 specular;
    float shininess;
    color3 emission;
    color3 ambient;

    Material() : diffuse(0.8, 0.8, 0.8), specular(0.0, 0.0, 0.0), shininess(0.0), emission(0.0, 0.0, 0.0), ambient(0.2, 0.2, 0.2) {}
    Material(color3 d, color3 s, float sh, color3 e, color3 a)
        : diffuse(d), specular(s), shininess(sh), emission(e), ambient(a) {}
};

#endif