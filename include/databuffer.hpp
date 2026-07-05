#pragma once

#include <glad/glad.h>
#include <vector>

struct AttribConfig
{
    GLuint slot;        // The STARTING layout(location = slot) in shader
    GLint count;        // Total components (e.g., 3 for vec3, 16 for mat4)
    GLenum type;        // Usually GL_FLOAT
    GLuint divisor = 0; // 0 for per-vertex data, 1 for per-instance data (matrices)
};

class DataBuffer
{
private:
    GLuint id;

public:
    template <typename T>
    DataBuffer(GLuint vao, const std::vector<T> &data, const AttribConfig &config, bool dynamic = false)
    {

        glGenBuffers(1, &id);
        glBindBuffer(GL_ARRAY_BUFFER, id);
        glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(T), data.data(), (dynamic == true) ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);

        glBindVertexArray(vao);

        // Calculate total bytes of one full vertex/instance element (Stride)
        GLsizei totalStride = sizeof(T);

        // If count is 16 (mat4), we need 4 slots (16 / 4 = 4)
        // If count is 9 (mat3), we need 3 slots (9 / 3 = 3)
        // For regular data (like vec3), maxComponents will be 3, slotsNeeded will be 1
        GLint componentsPerSlot = (config.count > 4) ? 4 : config.count;
        GLint slotsNeeded = config.count / componentsPerSlot;

        // Loop through and bind each slot required by the data type
        for (int i = 0; i < slotsNeeded; i++)
        {
            GLuint currentSlot = config.slot + i;

            glEnableVertexAttribArray(currentSlot);

            // Calculate the byte offset for this specific slot row
            // e.g., Row 0 starts at 0, Row 1 starts after 4 floats (16 bytes), etc.
            size_t offset = i * componentsPerSlot * sizeof(float);

            glVertexAttribPointer(
                currentSlot,
                componentsPerSlot,
                config.type,
                GL_FALSE,
                totalStride,
                (void *)offset);

            // Tell OpenGL how to advance the data.
            // For instanced matrices, this is usually 1 (advance once per instance drawn)
            if (config.divisor > 0)
            {
                glVertexAttribDivisor(currentSlot, config.divisor);
            }
        }

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    ~DataBuffer()
    {
        if (id != 0)
        {
            glDeleteBuffers(1, &id);
        }
    }

    DataBuffer(const DataBuffer &) = delete;
    DataBuffer &operator=(const DataBuffer &) = delete;

    GLuint getId() const { return id; }
};