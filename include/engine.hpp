#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <vector>
#include <iostream>
#include <random>

#include "input.h"
#include "camera.hpp"
#include "shader.hpp"
#include "meshgeometry.hpp"
#include "meshutils.hpp"
#include "databuffer.hpp"

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
    void processInput();
    void update(float deltaTime = 0.016f);
    void render();

    bool m_isRunning;
    GLFWwindow* m_window;
    int m_width;
    int m_height;

    MeshUtils::GLMesh m_mesh;
    int m_meshCount;
    float m_bounds;

    GLuint m_vao;
    GLuint m_vbo;
    bool m_needsGPUUpdate = true;

    Shader m_shader;
    Camera m_camera;

    std::vector<std::unique_ptr<IDataBuffer>> m_buffers;

    std::vector<glm::vec3> m_colors;
    std::vector<glm::mat4> m_models;
};