#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>

#include "input.h"
#include "shader.h"
#include "meshutils.h"
#include "meshgeometry.h"

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
    glfwSwapInterval(0);
    // V-Sync off.

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "GLAD initialization error." << std::endl;
        return -1;
    }
    glClearColor(0.15f, 0.15f, 0.15f, 1.0f);

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    // glViewport(width, height);

    // vertices
    Mesh sphere = MeshGen::CreateSphere(0.5f);

    // clang-format off
    // clang-format on


    MeshUtils::GLMesh readymesh = MeshUtils::uploadToGPU(sphere.vertices, sphere.indices);

    Shader shader("./shader/vertex.glsl", "./shader/fragment.glsl");
    GLint colorLoc = shader.getLocation("uColor");
    shader.use();

    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            break;
        }

        shader.use();
        shader.setVec3("uColor", glm::vec3(1.0f, 0.0f, 0.0f), colorLoc);
        glBindVertexArray(readymesh.VAO);
        glDrawElements(GL_TRIANGLE_FAN, readymesh.indexCount, GL_UNSIGNED_INT, (void*)0);

        glfwSwapBuffers(window);
        glfwPollEvents();

        glBindVertexArray(0);
    }

    glfwTerminate();
    return 0;
}