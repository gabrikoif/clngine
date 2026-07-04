#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>
#include <random> // Added for generating random positions

#include "input.h"
#include "shader.h"
#include "meshutils.h"
#include "meshgeometry.h"
#include "camera.h"

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
    GLFWwindow *window = glfwCreateWindow(mode->width, mode->height, "Collisiongine", monitor, nullptr);
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
    Mesh sphere = MeshGen::CreateSphere(2.0f);

    MeshUtils::GLMesh readymesh = MeshUtils::uploadToGPU(sphere.vertices, sphere.indices);

    // ==========================================
    // INSTANCING SETUP START
    // ==========================================
    const int NUM_SPHERES = 1000;
    const float BOUNDS = 500.0f; // Range from -25 to +25

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(-BOUNDS, BOUNDS);

    // 1. Generate initial random positions / matrices on CPU
    std::vector<glm::mat4> sphereMatrices;
    sphereMatrices.reserve(NUM_SPHERES);
    for (int i = 0; i < NUM_SPHERES; ++i)
    {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(dis(gen), dis(gen), dis(gen)));
        sphereMatrices.push_back(model);
    }

    std::vector<glm::vec3> sphereColors;
    sphereColors.reserve(NUM_SPHERES);
    for (int i = 0; i < NUM_SPHERES; ++i)
    {
        glm::vec3 color = glm::vec3(dis(gen), dis(gen), dis(gen));
        sphereColors.push_back(color);
    }

    // 2. Create the VBO to hold instance data on GPU
    GLuint instanceVBO;
    glGenBuffers(1, &instanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, NUM_SPHERES * sizeof(glm::mat4), nullptr, GL_DYNAMIC_DRAW);

    // 3. Link the matrix properties to the sphere's VAO
    glBindVertexArray(readymesh.VAO);
    std::size_t vec4Size = sizeof(glm::vec4);
    for (unsigned int i = 0; i < 4; i++)
    {
        glEnableVertexAttribArray(3 + i); // Slots 3, 4, 5, 6
        glVertexAttribPointer(3 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void *)(i * vec4Size));
        glVertexAttribDivisor(3 + i, 1); // Progresses 1 step per instance, not per vertex
    }
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    GLuint colorsVBO;
    glGenBuffers(1, &colorsVBO);
    glBindBuffer(GL_ARRAY_BUFFER, colorsVBO);
    glBufferData(GL_ARRAY_BUFFER, NUM_SPHERES * sizeof(glm::vec3), nullptr, GL_DYNAMIC_DRAW);

    glBindVertexArray(readymesh.VAO);

    glEnableVertexAttribArray(7);
    glVertexAttribPointer(7, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void *)0);
    glVertexAttribDivisor(7, 1);

    glBindVertexArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // ==========================================
    // INSTANCING SETUP END
    // ==========================================

    Shader shader("./shader/vertex.glsl", "./shader/fragment.glsl");

    Camera camera;
    camera.fov = 90.0f;
    camera.farPlane = 1500.0f;
    camera.nearPlane = 1.0f;
    globalCamPointer = &camera;

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
                glm::vec3 color = glm::vec3(dis(gen), dis(gen), dis(gen));
                sphereColors.push_back(color);
            }
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
        glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sphereMatrices.size() * sizeof(glm::mat4), sphereMatrices.data());
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindBuffer(GL_ARRAY_BUFFER, colorsVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, NUM_SPHERES * sizeof(glm::vec3), sphereColors.data());
        glBindBuffer(GL_ARRAY_BUFFER, 0);

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
    glDeleteBuffers(1, &instanceVBO);
    glDeleteBuffers(1, &colorsVBO);
    MeshUtils::deleteFromGPU(readymesh);

    glfwTerminate();
    return 0;
}