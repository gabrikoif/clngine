#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "input.h"
#include "camera.hpp"
#include "shader.hpp"

#include "meshgeometry.hpp"
#include "instanced_mesh_group.hpp"

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

    bool isPressed(int key);

    GLFWwindow* m_window = nullptr;
    bool m_isRunning;
    int m_width, m_height;

    Camera m_camera;
    Shader m_shader;

    InstancedMeshGroup m_group;
};
