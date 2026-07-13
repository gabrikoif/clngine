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
        glGenVertexArrays(1, &mesh.vao);
        glGenBuffers(1, &mesh.vbo);
        glGenBuffers(1, &mesh.ebo);

        // 2. Bind vao first
        glBindVertexArray(mesh.vao);

        // 3. Upload Vertex Data to vbo
        glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

        // 4. Upload Index Data to ebo
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ebo);
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

        // 6. Unbind vao (safely breaks the binding so nothing else accidentally changes it)
        glBindVertexArray(0);

        // Unbind buffers AFTER unbinding vao
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        return mesh;
    }

    void deleteFromGPU(GLMesh &mesh)
    {
        glDeleteVertexArrays(1, &mesh.vao);
        glDeleteBuffers(1, &mesh.vbo);
        glDeleteBuffers(1, &mesh.ebo);

        // Reset IDs to avoid accidental reuse
        mesh.vao = 0;
        mesh.vbo = 0;
        mesh.ebo = 0;
        mesh.indexCount = 0;
    }
}