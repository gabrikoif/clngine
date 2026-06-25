#pragma once

#include <vector>
#include <glm/glm.hpp>

namespace MeshUtils {

    // Clean vertex layout using GLM types
    struct Vertex {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 texCoords;
    };

    // A container to hold our OpenGL buffer IDs and draw count
    struct GLMesh {
        unsigned int VAO = 0;
        unsigned int VBO = 0;
        unsigned int EBO = 0;
        unsigned int indexCount = 0;
    };

    // Generates buffers, binds them, configures vertex attributes, and unbinds
    GLMesh uploadToGPU(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);

    // Safely deletes the buffers from GPU memory
    void deleteFromGPU(GLMesh& mesh);
}