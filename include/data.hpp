#include <glad/glad.h> // Or your preferred OpenGL loader
#include <glm/glm.hpp>
#include <vector>
#include <type_traits>

enum class BufferUsage { Static, Dynamic };

template <typename T>
class GLBuffer {
private:
    GLuint vbo_id = 0;
    GLenum usage_hint = GL_STATIC_DRAW;

public:
    // Default constructor: Safe for Engine member variables before GLAD is ready
    GLBuffer() = default;

    // Destructor: Clean up GPU memory safely
    ~GLBuffer() {
        if (vbo_id != 0) {
            glDeleteBuffers(1, &vbo_id);
        }
    }

    // Explicitly delete copy operations to prevent dual-deletion bugs
    GLBuffer(const GLBuffer&) = delete;
    GLBuffer& operator=(const GLBuffer&) = delete;

    // Allow moving resources safely
    GLBuffer(GLBuffer&& other) noexcept : vbo_id(other.vbo_id), usage_hint(other.usage_hint) {
        other.vbo_id = 0;
    }
    GLBuffer& operator=(GLBuffer&& other) noexcept {
        if (this != &other) {
            if (vbo_id != 0) glDeleteBuffers(1, &vbo_id);
            vbo_id = other.vbo_id;
            usage_hint = other.usage_hint;
            other.vbo_id = 0;
        }
        return *this;
    }

    // Call this in your Engine constructor AFTER GLAD is initialized
    void create(BufferUsage usage = BufferUsage::Static) {
        glGenBuffers(1, &vbo_id);
        usage_hint = (usage == BufferUsage::Dynamic) ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW;
    }

    // Upload (or re-upload) data to the GPU buffer
    void upload(const std::vector<T>& data) {
        glBindBuffer(GL_ARRAY_BUFFER, vbo_id);
        glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(T), data.data(), usage_hint);
    }

    // Updates a partial chunk of the buffer without reallocating
    void update_sub_data(const std::vector<T>& partial_data, size_t element_offset = 0) {
        glBindBuffer(GL_ARRAY_BUFFER, vbo_id);
        GLintptr byte_offset = element_offset * sizeof(T);
        GLsizeiptr byte_size = partial_data.size() * sizeof(T);
        glBufferSubData(GL_ARRAY_BUFFER, byte_offset, byte_size, partial_data.data());
    }

    // Links the buffer to the currently bound VAO
    // For normal types: pass layout, component count (e.g. 3 for vec3), and GL_FLOAT
    // For mat4: just pass the start layout location. Component count/type are ignored.
    void link_attribute(GLuint start_location, GLint component_count = 0, GLenum data_type = GL_FLOAT) {
        glBindBuffer(GL_ARRAY_BUFFER, vbo_id);

        // Check at compile time if this buffer holds matrices
        if constexpr (std::is_same_v<T, glm::mat4>) {
            // mat4 requires 4 consecutive attributes
            for (int i = 0; i < 4; i++) {
                GLuint current_location = start_location + i;
                glEnableVertexAttribArray(current_location);
                void* column_offset = (void*)(i * sizeof(glm::vec4));
                
                glVertexAttribPointer(
                    current_location, 
                    4, 
                    GL_FLOAT, 
                    GL_FALSE, 
                    sizeof(glm::mat4), 
                    column_offset
                );
            }
        } else {
            // Standard attribute linking for vec3, vec4, float, etc.
            glVertexAttribPointer(start_location, component_count, data_type, GL_FALSE, sizeof(T), (void*)0);
            glEnableVertexAttribArray(start_location);
        }
    }

    // Enables instancing for this attribute
    void set_instanced(GLuint start_location, GLuint divisor = 1) {
        if constexpr (std::is_same_v<T, glm::mat4>) {
            for (int i = 0; i < 4; i++) {
                glVertexAttribDivisor(start_location + i, divisor);
            }
        } else {
            glVertexAttribDivisor(start_location, divisor);
        }
    }

    void bind() const { glBindBuffer(GL_ARRAY_BUFFER, vbo_id); }
    void unbind() const { glBindBuffer(GL_ARRAY_BUFFER, 0); }
};