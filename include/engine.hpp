#pragma once

#include <glad/glad.h>
//
#include <GLFW/glfw3.h>

#include "camera.hpp"
#include "input.h"
#include "shader.hpp"

#include "instanced_mesh_group.hpp"
#include "meshgeometry.hpp"

#include "physics.hpp"

class Engine
{
public:
  Engine();
  ~Engine();

  bool initialize();
  void createAndLoad();
  void run();
  void shutdown();

private:
  void processInput(float deltaTime = 0.016f);
  void update(float deltaTime = 0.016f);
  void render();

  bool isPressed(int key);

  GLFWwindow* m_window = nullptr;
  bool m_isRunning;
  int m_width, m_height;

  Camera m_camera;
  Shader m_shader;

  InstancedMeshGroup m_group;

  Physics m_world;
};
