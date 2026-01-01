#define GLM_ENABLE_EXPERIMENTAL
#include <iostream>
#include <string>
#include <FreeImage.h>

#include "Scene.h"
#include "Parser.h"
#include "Renderer.h"

// The main entry point for the ray tracer program.
int main(int argc, char* argv[]) {
    // Argument validation
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <scene_file>" << std::endl;
        return 1;
    }

    // Scene Creation and Parsing
    Scene scene;
    Parser::parse_scene(argv[1], scene);

    // Rendering
    BYTE* pixels = Renderer::render(scene);

    // Image Saving
    FreeImage_Initialise();
    FIBITMAP* img = FreeImage_ConvertFromRawBits(
        pixels,
        scene.width,
        scene.height,
        scene.width * 3,
        24,
        0xFF0000, 0x00FF00, 0x0000FF,
        true
    );

    if (!FreeImage_Save(FIF_PNG, img, scene.output_filename.c_str(), 0)) {
        std::cerr << "Error: Could not save image " << scene.output_filename << std::endl;
    }

    // Cleanup
    FreeImage_Unload(img);
    FreeImage_DeInitialise();
    delete[] pixels;

    return 0;
}
