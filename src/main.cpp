#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>
#include <random> // Added for generating random positions

#include "input.h"
#include "shader.hpp"
#include "meshutils.hpp"
#include "meshgeometry.hpp"
#include "camera.hpp"
#include "databuffer.hpp"

int main()
{
    if (!glfwInit())
    {
        std::cout << "GLFW initializiation error." << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWmonitor *monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode *mode = glfwGetVideoMode(monitor);
    GLFWwindow *window = glfwCreateWindow(mode->width, mode->height, "Collisiongine", nullptr, nullptr);
    if (window == nullptr)
    {
        std::cout << "GLFW Window initialization error." << std::endl;
        glfwTerminate();
        return -1;
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
        return -1;
    }
    glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    // vertices
    Mesh sphere = MeshGen::CreateSphere(10.0f);

    MeshUtils::GLMesh readymesh = MeshUtils::uploadToGPU(sphere.vertices, sphere.indices);

    // ==========================================
    // INSTANCING SETUP START
    // ==========================================
    const int NUM_SPHERES = 100;
    const float BOUNDS = 500.0f; // Range from -25 to +25

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

    // 2. Create the VBO to hold instance data on GPU
    AttribConfig modelConfig;
    modelConfig.slot = 3;
    modelConfig.count = 16;
    modelConfig.divisor = 1;
    modelConfig.type = GL_FLOAT;

    DataBuffer modelBuffer(readymesh.VAO, sphereMatrices, modelConfig, 1);

    AttribConfig colorConfig;
    colorConfig.slot = 7;
    colorConfig.count = 3;
    colorConfig.divisor = 1;
    colorConfig.type = GL_FLOAT;

    DataBuffer colorBuffer(readymesh.VAO, sphereColors, colorConfig, 1);

    // ==========================================
    // INSTANCING SETUP END
    // ==========================================

    Shader shader("./shader/vertex.glsl", "./shader/fragment.glsl");

    Camera camera;
    camera.fov = 103.0f;
    camera.farPlane = 1500.0f;
    camera.nearPlane = 1.0f;
    globalCamPointer = &camera;

    bool needsGPUUpdate = true;
    while (!glfwWindowShouldClose(window))
    {
        // Fetch fresh dimensions in case of resizing
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            break;
        }

        handle_input(window);

        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
        {
            sphereColors = {};
            sphereColors.reserve(NUM_SPHERES);
            for (int i = 0; i < NUM_SPHERES; ++i)
            {
                glm::vec3 color = glm::vec3(zero_to_one(gen), zero_to_one(gen), zero_to_one(gen));
                sphereColors.push_back(color);
            }
            needsGPUUpdate = true;
        }

        float aspectRatio = (float)width / (float)height;

        // Get camera matrices
        glm::mat4 view = camera.genView();
        glm::mat4 projection = camera.genProj(aspectRatio);

        // UPDATE PHASE (PHYSICS ON THE CPU)
        // TODO: Add Physics class in physics.cpp so that:
        // you only have to write:
        // world.update();

        // 2. UPLOAD PHASE (CPU -> GPU)
        if (needsGPUUpdate)
        {
            glBindBuffer(GL_ARRAY_BUFFER, modelBuffer.getId());
            glBufferSubData(GL_ARRAY_BUFFER, 0, sphereMatrices.size() * sizeof(glm::mat4), sphereMatrices.data());
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glBindBuffer(GL_ARRAY_BUFFER, colorBuffer.getId());
            glBufferSubData(GL_ARRAY_BUFFER, 0, NUM_SPHERES * sizeof(glm::vec3), sphereColors.data());
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            
            needsGPUUpdate = false;
        }

        // 3. RENDER PHASE (GPU)
        shader.use();
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);

        glBindVertexArray(readymesh.VAO);
        glDrawElementsInstanced(
            GL_TRIANGLES,
            readymesh.indexCount, // Assuming GLMesh stores your index count
            GL_UNSIGNED_INT,
            0,
            NUM_SPHERES);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Clean up our allocated buffer before closing down
    MeshUtils::deleteFromGPU(readymesh);

    glfwTerminate();
    return 0;
}