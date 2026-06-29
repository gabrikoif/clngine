#include "meshgeometry.h"
#include <cmath>

namespace MeshGen {

    Mesh CreateSphere(float radius, unsigned int segments, unsigned int rings) {
        Mesh mesh;
        
        // Generate Vertices
        for (unsigned int y = 0; y <= rings; ++y) {
            for (unsigned int x = 0; x <= segments; ++x) {
                
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
        for (unsigned int y = 0; y < rings; ++y) {
            for (unsigned int x = 0; x < segments; ++x) {
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
}