#define _USE_MATH_DEFINES
#include "meshgeometry.hpp"
#include <cmath>

Mesh MeshGen::CreateSphere(float radius, unsigned int segments, unsigned int rings)
{
    Mesh mesh;

    // Generate Vertices
    for (unsigned int y = 0; y <= rings; ++y)
    {
        for (unsigned int x = 0; x <= segments; ++x)
        {

            float xSegment = (float)x / (float)segments;
            float ySegment = (float)y / (float)rings;

            float xPos = std::cos(xSegment * 2.0f * M_PI) * std::sin(ySegment * M_PI);
            float yPos = std::cos(ySegment * M_PI); // Top to bottom
            float zPos = std::sin(xSegment * 2.0f * M_PI) * std::sin(ySegment * M_PI);

            Vertex v;
            v.position = radius * glm::vec3(xPos, yPos, zPos);
            v.normal = glm::vec3(xPos, yPos, zPos);
            v.texCoords = glm::vec2(xSegment, ySegment);

            mesh.vertices.push_back(v);
        }
    }

    // Generate Indices
    for (unsigned int y = 0; y < rings; ++y)
    {
        for (unsigned int x = 0; x < segments; ++x)
        {
            unsigned int currentIdx = y * (segments + 1) + x;
            unsigned int nextIdx = currentIdx + segments + 1;

            // Triangle 1
            mesh.indices.push_back(currentIdx);
            mesh.indices.push_back(nextIdx);
            mesh.indices.push_back(currentIdx + 1);

            // Triangle 2
            mesh.indices.push_back(currentIdx + 1);
            mesh.indices.push_back(nextIdx);
            mesh.indices.push_back(nextIdx + 1);
        }
    }

    return mesh;
}

Mesh MeshGen::CreateCube(float size)
{
    Mesh mesh;
    float half = size / 2.0f;

    // Define the 24 vertices matching your Vertex struct layout
    // Format: { Position (X,Y,Z), Normal (X,Y,Z), TexCoords (U,V) }
    struct CubeVertexData
    {
        glm::vec3 pos;
        glm::vec3 norm;
        glm::vec2 uv;
    };

    std::vector<CubeVertexData> rawVertices = {
        // Front Face
        {{-half, -half, half}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},
        {{half, -half, half}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},
        {{half, half, half}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
        {{-half, half, half}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},

        // Back Face
        {{-half, -half, -half}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f}},
        {{-half, half, -half}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f}},
        {{half, half, -half}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}},
        {{half, -half, -half}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}},

        // Top Face
        {{-half, half, -half}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},
        {{-half, half, half}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
        {{half, half, half}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
        {{half, half, -half}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},

        // Bottom Face
        {{-half, -half, -half}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f}},
        {{half, -half, -half}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f}},
        {{half, -half, half}, {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f}},
        {{-half, -half, half}, {0.0f, -1.0f, 0.0f}, {0.0f, 1.0f}},

        // Right Face
        {{half, -half, -half}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
        {{half, half, -half}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},
        {{half, half, half}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},
        {{half, -half, half}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},

        // Left Face
        {{-half, -half, -half}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
        {{-half, -half, half}, {-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
        {{-half, half, half}, {-1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},
        {{-half, half, -half}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}}};

    // Convert to your engine's Vertex type
    mesh.vertices.reserve(rawVertices.size());
    for (const auto &v : rawVertices)
    {
        Vertex vertex;
        vertex.position = v.pos;
        vertex.normal = v.norm;
        vertex.texCoords = v.uv;
        mesh.vertices.push_back(vertex);
    }

    // Index mappings (2 triangles per face, 6 faces total)
    mesh.indices = {
        0, 1, 2, 2, 3, 0,       // Front
        4, 5, 6, 6, 7, 4,       // Back
        8, 9, 10, 10, 11, 8,    // Top
        12, 13, 14, 14, 15, 12, // Bottom
        16, 17, 18, 18, 19, 16, // Right
        20, 21, 22, 22, 23, 20  // Left
    };

    return mesh;
}