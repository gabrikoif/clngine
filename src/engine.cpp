#include "engine.hpp"

Engine::Engine() : m_isRunning(false) {};
Engine::~Engine() {};

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

bool Engine::initialize()
{
    bool wantsFullscreen = false;
    if (!glfwInit())
    {
        std::cout << "GLFW initialization failed\n";
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWmonitor *monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode *mode = glfwGetVideoMode(monitor);
    GLFWwindow *window = glfwCreateWindow(
        mode->width,
        mode->height,
        "CLngine",
        (wantsFullscreen) ? monitor : nullptr,
        nullptr);

    if (window == nullptr)
    {
        std::cout << "GLFW Window initialization failed\n";
        return false;
    }

    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSwapInterval(1);
    // V-Sync on.

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "GLAD initialization error." << std::endl;
        return false;
    }
    glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
    // Background color.
    glEnable(GL_DEPTH_TEST);

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    globalCamPointer = &m_camera;

    m_window = window;
    glfwSetWindowUserPointer(m_window, this);
    return true;
}

void Engine::createAndLoad()
{
    Mesh ballMesh = MeshGen::CreateSphere(1.0f);
    MeshUtils::GLMesh GPUBallMesh = MeshUtils::uploadToGPU(ballMesh.vertices, ballMesh.indices);

    m_meshCount = 10000;
    m_bounds = 50.0f;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> bounds(-m_bounds, m_bounds);
    std::uniform_real_distribution<float> zero_to_one(0.0f, 1.0f);

    // 1. Generate initial random positions / matrices on CPU
    std::vector<glm::mat4> sphereMatrices;
    sphereMatrices.reserve(m_meshCount);
    for (int i = 0; i < m_meshCount; ++i)
    {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(bounds(gen), bounds(gen), bounds(gen)));
        sphereMatrices.push_back(model);
    }

    std::vector<glm::vec3> sphereColors;
    sphereColors.reserve(m_meshCount);
    for (int i = 0; i < m_meshCount; ++i)
    {
        glm::vec3 color = glm::vec3(zero_to_one(gen), zero_to_one(gen), zero_to_one(gen));
        sphereColors.push_back(color);
    }

    m_mesh = GPUBallMesh;
    m_colors = std::move(sphereColors);
    m_models = std::move(sphereMatrices);

    // 2. Create the VBO to hold instance data on GPU
    AttribConfig modelConfig;
    modelConfig.slot = 3;
    modelConfig.count = 16;
    modelConfig.divisor = 1;
    modelConfig.type = GL_FLOAT;

    m_buffers.push_back(
        std::make_unique<DataBuffer<glm::mat4>>(m_mesh.VAO, m_models, modelConfig, false));

    AttribConfig colorConfig;
    colorConfig.slot = 7;
    colorConfig.count = 3;
    colorConfig.divisor = 1;
    colorConfig.type = GL_FLOAT;

    m_buffers.push_back(
        std::make_unique<DataBuffer<glm::vec3>>(m_mesh.VAO, m_colors, colorConfig, true));
    
    // Data done.

    m_camera.fov = 103.0f;
    m_camera.farPlane = 1500.0f;
    m_camera.nearPlane = 1.0f;

    m_shader.load("shader/vertex.glsl", "shader/fragment.glsl");
    // Shaders and camera ready.
}

void Engine::run()
{
    while (!glfwWindowShouldClose(m_window))
    {
        processInput();
        update();
        render();
    }
}

void Engine::shutdown()
{
    MeshUtils::deleteFromGPU(m_mesh);
    glfwTerminate();
}

// Private Methods

void Engine::processInput()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution zero_to_one(0.0f, 1.0f);

    if (glfwGetKey(m_window, GLFW_KEY_R) == GLFW_PRESS)
    {

        for (int i = 0; i < m_meshCount; ++i)
        {
            m_colors[i] = glm::vec3(zero_to_one(gen), zero_to_one(gen), zero_to_one(gen));
        }
        m_needsGPUUpdate = true;
    }

    if (glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(m_window, 1);
    }

    handle_input(m_window);
}

void Engine::update(float DeltaTime)
{
    // TODO: Add physics CPU part here.
    // Ideally, m_world.update(DeltaTime);

    if (m_needsGPUUpdate)
    {
        m_buffers[0]->updateGPU(m_models.data(), m_models.size() * sizeof(glm::mat4));
        m_buffers[1]->updateGPU(m_colors.data(), m_colors.size() * sizeof(glm::vec3));
        m_needsGPUUpdate = false;
    }
}

void Engine::render()
{
    glfwGetFramebufferSize(m_window, &m_width, &m_height);
    glViewport(0, 0, m_width, m_height);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_shader.use();

    float aspectRatio = (float)m_width / (float)m_height;
    glm::mat4 view = m_camera.genView();
    glm::mat4 projection = m_camera.genProj(aspectRatio);

    m_shader.setMat4("view", view);
    m_shader.setMat4("projection", projection);

    glBindVertexArray(m_mesh.VAO);
    glDrawElementsInstanced(
        GL_TRIANGLES,
        m_mesh.indexCount, // Assuming GLMesh stores your index count
        GL_UNSIGNED_INT,
        0,
        m_meshCount);
    glBindVertexArray(0);

    glfwSwapBuffers(m_window);
    glfwPollEvents();
}
