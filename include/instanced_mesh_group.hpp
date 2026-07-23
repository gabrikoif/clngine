
// instanced_mesh_group.hpp
#pragma once
#include "data.hpp"
#include "gameobj.hpp"
#include "meshutils.hpp"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <random>
#include <vector>

class InstancedMeshGroup
{
public:
  InstancedMeshGroup() = default;
  ~InstancedMeshGroup() { destroy(); }
  float m_bounds;

  void initialize(MeshUtils::GLMesh baseMesh, int instanceCount,
                  float boundsRange, float p_mass, float p_radius)
  {
    m_mesh = baseMesh;
    m_count = instanceCount;

    m_mass = p_mass;
    m_radius = p_radius;

    m_bounds = boundsRange;

    m_models.reserve(m_count);
    m_colors.reserve(m_count);
    m_objs.reserve(m_count);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> bounds(-boundsRange, boundsRange);
    std::uniform_real_distribution<float> zero_to_one(0.0f, 1.0f);
    std::uniform_real_distribution<float> radii(0.01 * boundsRange, 0.1 * boundsRange);
    std::uniform_real_distribution<float> speed(-50.0f, 50.0f);

    // Generate initial data
    for (int i = 0; i < m_count; ++i)
    {
      float radius = m_radius;
      float mass = 5 * radius;
      GameObj curr_obj(mass, radius);
      curr_obj.pos = glm::vec3(bounds(gen), abs(bounds(gen)), bounds(gen));
      curr_obj.vel = glm::vec3(speed(gen), speed(gen), speed(gen));
      glm::mat4 model = glm::translate(glm::mat4(1.0f), curr_obj.pos);
      model = glm::scale(model, glm::vec3(radius));
      m_models.push_back(model);
      m_colors.push_back(
          glm::vec3(zero_to_one(gen), zero_to_one(gen), zero_to_one(gen)));
      m_objs.push_back(curr_obj);
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

  void rerun()
  {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> bounds(-m_bounds, m_bounds);
    std::uniform_real_distribution<float> zero_to_one(0.0f, 1.0f);
    std::uniform_real_distribution<float> radii(0.01 * m_bounds, 0.1 * m_bounds);
    std::uniform_real_distribution<float> speed(-50.0f, 50.0f);

    // Generate initial data
    for (int i = 0; i < m_count; ++i)
    {
      float radius = radii(gen);
      float mass = 5 * radius;
      GameObj &curr_obj = m_objs[i];
      curr_obj.pos = glm::vec3(bounds(gen), abs(bounds(gen)), bounds(gen));
      curr_obj.vel = glm::vec3(speed(gen), speed(gen), speed(gen));
      glm::mat4 model = glm::translate(glm::mat4(1.0f), curr_obj.pos);
      model = glm::scale(model, glm::vec3(radius));
      m_models[i] = model;
      m_colors[i] =
          glm::vec3(zero_to_one(gen), zero_to_one(gen), zero_to_one(gen));
    }
    m_needsGPUUpdate = true;
  }

  void updateMatrices()
  {
    int i = 0;
    if (m_models.size() == 0)
      return;
    for (auto &obj : m_objs)
    {
      glm::mat4 transform = glm::mat4(1.0f);
      transform = glm::translate(transform, obj.pos);
      transform = glm::scale(transform, glm::vec3(obj.radius));
      m_models[i] = transform;
      ++i;
    }
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
    if (m_count == 0)
      return;
    glBindVertexArray(m_mesh.vao);
    glDrawElementsInstanced(GL_TRIANGLES, m_mesh.indexCount, GL_UNSIGNED_INT, 0,
                            m_count);
    glBindVertexArray(0);
  }

  void destroy()
  {
    MeshUtils::deleteFromGPU(m_mesh);
    // Clean up buffers here if your Buffer class doesn't do it in its
    // destructor
  }

  std::vector<GameObj> &getObjs() { return m_objs; }

private:
  MeshUtils::GLMesh m_mesh;
  int m_count = 0;
  bool m_needsGPUUpdate = false;

  float m_radius;
  float m_mass;
  std::vector<GameObj> m_objs;

  // CPU Mirror
  std::vector<glm::mat4> m_models;
  std::vector<glm::vec3> m_colors;

  // GPU Buffers
  GLBuffer<glm::mat4> m_modelBuffer;
  GLBuffer<glm::vec3> m_colorBuffer;
};