#include <glad/glad.h>
// Header with the structs.
#include "meshutils.hpp"

namespace MeshUtils
{

    GLMesh uploadToGPU(const std::vector<Vertex> &vertices, const std::vector<unsigned int> &indices)
    {
        GLMesh mesh;
        mesh.indexCount = indices.size();

        // 1. Generate IDs
        glGenVertexArrays(1, &mesh.VAO);
        glGenBuffers(1, &mesh.VBO);
        glGenBuffers(1, &mesh.EBO);

        // 2. Bind VAO first
        glBindVertexArray(mesh.VAO);

        // 3. Upload Vertex Data to VBO
        glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

        // 4. Upload Index Data to EBO
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        // 5. Configure Vertex Attributes (Assuming layout 0 = Pos, 1 = Normal, 2 = TexCoords)

        // Position Attribute
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, position));

        // Normal Attribute
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, normal));

        // Texture Coordinate Attribute
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, texCoords));

        // 6. Unbind VAO (safely breaks the binding so nothing else accidentally changes it)
        glBindVertexArray(0);

        // Unbind buffers AFTER unbinding VAO
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        return mesh;
    }

    void deleteFromGPU(GLMesh &mesh)
    {
        glDeleteVertexArrays(1, &mesh.VAO);
        glDeleteBuffers(1, &mesh.VBO);
        glDeleteBuffers(1, &mesh.EBO);

        // Reset IDs to avoid accidental reuse
        mesh.VAO = 0;
        mesh.VBO = 0;
        mesh.EBO = 0;
        mesh.indexCount = 0;
    }
}