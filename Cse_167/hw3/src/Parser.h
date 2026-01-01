#ifndef PARSER_H
#define PARSER_H

#include <string>
#include "Scene.h"

// The Parser namespace contains the logic for reading and interpreting a scene description file.
namespace Parser {
    // Reads a scene file and populates the Scene object with all settings, geometry, and lights.
    void parse_scene(const std::string& filename, Scene& scene);
}

#endif
