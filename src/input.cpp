#include "input.h"

Camera *globalCamPointer = nullptr;
float lastX = 400.0f;
float lastY = 300.0f;
bool firstMouse = true;

float walkSens = 5.0f;

void mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
    (void)window;

    xpos = static_cast<float>(xpos);
    ypos = static_cast<float>(ypos);

    if (firstMouse)
    {
        lastX = xpos; lastY = ypos; firstMouse = false;
    }

    float xOffset = xpos - lastX;
    float yOffset = lastY - ypos;
    // openGL preference to count top as y = 0.
    lastX = xpos; lastY = ypos;

    if (globalCamPointer != nullptr)
    {
        globalCamPointer->handleMouseMovement(xOffset, yOffset);
    }
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
    float aspectRatio = (float)width / (float)height;
}

void handle_input(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        globalCamPointer->cameraPos += walkSens * globalCamPointer->viewDir;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        globalCamPointer->cameraPos -= walkSens * globalCamPointer->viewDir;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        globalCamPointer->cameraPos -= walkSens * glm::cross(globalCamPointer->viewDir, glm::vec3(0.0f, 1.0f, 0.0f));
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        globalCamPointer->cameraPos += walkSens * glm::cross(globalCamPointer->viewDir, glm::vec3(0.0f, 1.0f, 0.0f));
}
