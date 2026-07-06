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
    void update(float deltaTime);
    void render();

    bool m_isRunning;
    GLFWwindow* m_window;
    MeshUtils::GLMesh m_mesh;
    GLuint m_vao;
    GLuint m_vbo;

    Shader m_shader;
    Camera m_camera;

    std::vector<std::unique_ptr<IDataBuffer>> m_buffers;

    std::vector<glm::vec3> m_colors;
    std::vector<glm::mat4> m_models;
};