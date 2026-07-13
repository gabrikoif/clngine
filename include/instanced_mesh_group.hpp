
// instanced_mesh_group.hpp
#pragma once
#include <vector>
#include <random>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>
#include "meshutils.hpp"
#include "data.hpp"

class InstancedMeshGroup
{
public:
    InstancedMeshGroup() = default;
    ~InstancedMeshGroup() { destroy(); }

    void initialize(MeshUtils::GLMesh baseMesh, int instanceCount, float boundsRange)
    {
        m_mesh = baseMesh;
        m_count = instanceCount;

        m_models.reserve(m_count);
        m_colors.reserve(m_count);

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> bounds(-boundsRange, boundsRange);
        std::uniform_real_distribution<float> zero_to_one(0.0f, 1.0f);

        // Generate initial data
        for (int i = 0; i < m_count; ++i)
        {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(bounds(gen), bounds(gen), bounds(gen)));
            m_models.push_back(model);
            m_colors.push_back(glm::vec3(zero_to_one(gen), zero_to_one(gen), zero_to_one(gen)));
        }

        // Setup buffers
        m_colorBuffer.create(BufferUsage::Dynamic);
        m_modelBuffer.create();

        m_colorBuffer.upload(m_colors);
        m_modelBuffer.upload(m_models);

        // Bind attributes to the base mesh VAO
        glBindVertexArray(m_mesh.vao);

        m_colorBuffer.link_attribute(7, 3, GL_FLOAT);
        m_colorBuffer.set_instanced(7);

        m_modelBuffer.link_attribute(3);
        m_modelBuffer.set_instanced(3);

        glBindVertexArray(0);
    }

    void randomizeColors()
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> zero_to_one(0.0f, 1.0f);

        for (int i = 0; i < m_count; ++i)
        {
            m_colors[i] = glm::vec3(zero_to_one(gen), zero_to_one(gen), zero_to_one(gen));
        }
        m_needsGPUUpdate = true;
    }

    void setTransforms(std::vector<glm::mat4>&& newMatrices)
    {
        m_models = std::move(newMatrices);
        m_needsGPUUpdate = true;
    }

    void updateGPU()
    {
        if (!m_needsGPUUpdate)
            return;

        m_colorBuffer.update_sub_data(m_colors, 0);
        m_modelBuffer.update_sub_data(m_models, 0);

        m_needsGPUUpdate = false;
    }

    void draw() const
    {
        if (m_count == 0) return;
        glBindVertexArray(m_mesh.vao);
        glDrawElementsInstanced(
            GL_TRIANGLES,
            m_mesh.indexCount,
            GL_UNSIGNED_INT,
            0,
            m_count);
        glBindVertexArray(0);
    }

    void destroy()
    {
        MeshUtils::deleteFromGPU(m_mesh);
        // Clean up buffers here if your Buffer class doesn't do it in its destructor
    }

private:
    MeshUtils::GLMesh m_mesh;
    int m_count = 0;
    bool m_needsGPUUpdate = false;

    // CPU Mirror
    std::vector<glm::mat4> m_models;
    std::vector<glm::vec3> m_colors;

    // GPU Buffers
    GLBuffer<glm::mat4> m_modelBuffer;
    GLBuffer<glm::vec3> m_colorBuffer;
};