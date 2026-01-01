#ifndef RENDERER_H
#define RENDERER_H

#include "Scene.h"
#include <FreeImage.h>

// The Renderer namespace contains the logic for generating an image from a Scene object.
namespace Renderer {
    // Renders the scene and returns a pointer to a dynamically allocated pixel buffer (in BGR order).
    BYTE* render(const Scene& scene);
}

#endif
