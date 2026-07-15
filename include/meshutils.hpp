#pragma once

#include <vector>
#include "graphics.h"

namespace MeshUtils {
    // A container to hold our OpenGL buffer IDs and draw count
    struct GLMesh {
        unsigned int vao = 0;
        unsigned int vbo = 0;
        unsigned int ebo = 0;
        unsigned int indexCount = 0;
    };

    // Generates buffers, binds them, configures vertex attributes, and unbinds
    GLMesh uploadToGPU(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);

    // Safely deletes the buffers from GPU memory
    void deleteFromGPU(GLMesh& mesh);
}