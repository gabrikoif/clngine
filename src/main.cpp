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
    glfwSwapInterval(0);
    // V-Sync off.

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "GLAD initialization error." << std::endl;
        return -1;
    }
    glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    // glViewport(width, height);

    // vertices
    Mesh sphere = MeshGen::CreateSphere(2.0f);

    // clang-format off
    // clang-format on


    MeshUtils::GLMesh readymesh = MeshUtils::uploadToGPU(sphere.vertices, sphere.indices);

    Shader shader("./shader/vertex.glsl", "./shader/fragment.glsl");
    GLint colorLoc = shader.getLocation("uColor");
    GLint modelLoc = shader.getLocation("model");
    GLint viewLoc = shader.getLocation("view");
    GLint projectionLoc = shader.getLocation("projection");
    shader.use();

    Camera camera;
    camera.fov = 45.0f;
    camera.farPlane = 1000.0f;
    camera.nearPlane = 1.0f;
    globalCamPointer = &camera;

    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            break;
        }

        handle_input(window);

        float aspectRatio = (float)width / (float)height;

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(10, 5, 0));
        glm::mat4 view = camera.genView();
        glm::mat4 projection = camera.genProj(aspectRatio);

        shader.use();
        shader.setMat4("model", model, modelLoc);
        shader.setMat4("view", view, viewLoc);
        shader.setMat4("projection", projection, projectionLoc);
        shader.setVec3("uColor", glm::vec3(1.0f, 1.0f, 1.0f), colorLoc);
        glBindVertexArray(readymesh.VAO);
        glDrawElements(GL_TRIANGLES, readymesh.indexCount, GL_UNSIGNED_INT, (void*)0);

        glfwSwapBuffers(window);
        glfwPollEvents();

        glBindVertexArray(0);
    }

    glfwTerminate();
    return 0;
}