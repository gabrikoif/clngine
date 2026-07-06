#pragma once

#include <glad/glad.h>
#include <vector>

struct AttribConfig
{
    GLuint slot;        // The STARTING layout(location = slot) in shader
    GLint count;        // Total components (e.g., 3 for vec3, 16 for mat4)
    GLenum type;        // Usually GL_FLOAT
    GLuint divisor = 0; // 0 for per-vertex data, 1 for per-instance data
};

class IDataBuffer 
{
public:
    virtual ~IDataBuffer() = default;
    virtual GLuint getId() const = 0;
    virtual void updateGPU(const void* data, size_t sizeInBytes) = 0; 
};

template <typename T>
class DataBuffer : public IDataBuffer
{
private:
    GLuint id;
    std::vector<T> cpuData; 
    AttribConfig config;

public:
    DataBuffer(GLuint vao, const std::vector<T> &data, const AttribConfig &config, bool dynamic = false)
        : cpuData(data), config(config), id(0)
    {
        // 1. Generate and initialize the VBO
        glGenBuffers(1, &id);
        glBindBuffer(GL_ARRAY_BUFFER, id);
        
        // Allocate the initial memory block so glBufferSubData works down the line
        GLenum usage = dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW;
        glBufferData(GL_ARRAY_BUFFER, cpuData.size() * sizeof(T), cpuData.data(), usage);

        // 2. CRITICAL FIX: Bind the target VAO so attribute layouts attach to it!
        glBindVertexArray(vao);

        // 3. Configure the Vertex Attributes
        if (config.count == 16)
        {
            // A mat4 is too big for a single attribute slot, so we split it into 
            // 4 consecutive slots (each containing a vec4 / 4 components)
            GLsizei stride = sizeof(T); // The entire size of a mat4
            for (unsigned int i = 0; i < 4; i++)
            {
                GLuint targetSlot = config.slot + i;
                glEnableVertexAttribArray(targetSlot);
                glVertexAttribPointer(
                    targetSlot,
                    4,                          // 4 floats per row vector
                    config.type,
                    GL_FALSE,
                    stride,
                    (void*)(i * 4 * sizeof(float)) // Offset shifts by 1 row (4 floats) each iteration
                );
                
                if (config.divisor > 0)
                {
                    glVertexAttribDivisor(targetSlot, config.divisor);
                }
            }
        }
        else
        {
            // Standard single-slot handling (for vec3 colors, vec2 texcoords, etc.)
            glEnableVertexAttribArray(config.slot);
            glVertexAttribPointer(
                config.slot,
                config.count,
                config.type,
                GL_FALSE,
                sizeof(T), // Stride matches the element size
                (void*)0
            );

            if (config.divisor > 0)
            {
                glVertexAttribDivisor(config.slot, config.divisor);
            }
        }

        // 4. Clean up state bindings safely
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    ~DataBuffer() override
    {
        if (id != 0)
            glDeleteBuffers(1, &id);
    }

    std::vector<T> &getData() { return cpuData; }

    GLuint getId() const override { return id; }

    void updateGPU(const void* data, size_t sizeInBytes) override
    {
        glBindBuffer(GL_ARRAY_BUFFER, id);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeInBytes, data);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
};