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
    Mesh sphere = MeshGen::CreateCube(1.0f);

    MeshUtils::GLMesh readymesh = MeshUtils::uploadToGPU(sphere.vertices, sphere.indices);

    // ==========================================
    // INSTANCING SETUP START
    // ==========================================
    const int NUM_SPHERES = 10000;
    const float BOUNDS = 50.0f; // Range from -25 to +25

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(-BOUNDS, BOUNDS);
    std::uniform_real_distribution<float> zero_to_one(0.0f, 1.0f);

    // 1. Generate initial random positions / matrices on CPU
    std::vector<glm::mat4> sphereMatrices;
    sphereMatrices.reserve(NUM_SPHERES);
    for (int i = 0; i < NUM_SPHERES; ++i)
    {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(dis(gen), dis(gen), dis(gen)));
        // model = glm::rotate(model, 45.0f, glm::vec3(zero_to_one(gen), zero_to_one(gen), zero_to_one(gen)));
        // When time comes and I have texcoords
        sphereMatrices.push_back(model);
    }

    std::vector<glm::vec3> sphereColors;
    sphereColors.reserve(NUM_SPHERES);
    for (int i = 0; i < NUM_SPHERES; ++i)
    {
        glm::vec3 color = glm::vec3(zero_to_one(gen), zero_to_one(gen), zero_to_one(gen));
        sphereColors.push_back(color);
    }

    m_mesh = readymesh;
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

    m_camera.fov = 103.0f;
    m_camera.farPlane = 1500.0f;
    m_camera.nearPlane = 1.0f;

    m_shader.load("shader/vertex.glsl", "shader/fragment.glsl");
}

void Engine::run()
{
    const int NUM_SPHERES = 10000;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution zero_to_one(0.0f, 1.0f);
    int width, height;
    bool needsGPUUpdate = true;
    while (!glfwWindowShouldClose(m_window))
    {
        // Fetch fresh dimensions in case of resizing
        glfwGetFramebufferSize(m_window, &width, &height);
        glViewport(0, 0, width, height);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            break;
        }

        handle_input(m_window);

        if (glfwGetKey(m_window, GLFW_KEY_R) == GLFW_PRESS)
        {

            for (int i = 0; i < NUM_SPHERES; ++i)
            {
                m_colors[i] = glm::vec3(zero_to_one(gen), zero_to_one(gen), zero_to_one(gen));
            } 
            needsGPUUpdate = true;
        }

        float aspectRatio = (float)width / (float)height;

        // Get camera matrices
        glm::mat4 view = m_camera.genView();
        glm::mat4 projection = m_camera.genProj(aspectRatio);

        // UPDATE PHASE (PHYSICS ON THE CPU)
        // TODO: Add Physics class in physics.cpp so that:
        // you only have to write:
        // world.update();

        // 2. UPLOAD PHASE (CPU -> GPU)
        if (needsGPUUpdate)
        {
            m_buffers[0]->updateGPU(m_models.data(), m_models.size() * sizeof(glm::mat4));
            m_buffers[1]->updateGPU(m_colors.data(), m_colors.size() * sizeof(glm::vec3));
            needsGPUUpdate = false;
        }

        // 3. RENDER PHASE (GPU)
        m_shader.use();
        m_shader.setMat4("view", view);
        m_shader.setMat4("projection", projection);

        glBindVertexArray(m_mesh.VAO);
        glDrawElementsInstanced(
            GL_TRIANGLES,
            m_mesh.indexCount, // Assuming GLMesh stores your index count
            GL_UNSIGNED_INT,
            0,
            NUM_SPHERES);
        glBindVertexArray(0);

        glfwSwapBuffers(m_window);
        glfwPollEvents();
    }
}

void Engine::shutdown()
{
    MeshUtils::deleteFromGPU(m_mesh);
    glfwTerminate();
}
