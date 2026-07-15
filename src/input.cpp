#include "input.h"

static float lastX = 0.0f;
static float lastY = 0.0f;
static bool firstMouse = true;

Camera* globalCamPtr = nullptr;

void mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
    (void)window;

    xpos = static_cast<float>(xpos);
    ypos = static_cast<float>(ypos);
    if (firstMouse)
    {
        lastX = xpos; 
        lastY = ypos; 
        firstMouse = false;
    }

    float xOffset = xpos - lastX;
    float yOffset = lastY - ypos;
    // openGL preference to count top as y = 0.
    lastX = xpos; 
    lastY = ypos;

    if (globalCamPtr)
    {
        globalCamPtr->handleMouseMovement(xOffset, yOffset);
    }
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
    float aspectRatio = (float)width / (float)height;
}

// Returns the pressed key code instantly, or -1 if no key is pressed
int glfw_getch_nonblocking(GLFWwindow* window) {
    // 1. Refresh GLFW's internal input state for this frame
    glfwPollEvents(); 

    // 2. Scan for the first key currently being pressed
    for (int i = GLFW_KEY_SPACE; i <= GLFW_KEY_LAST; i++) {
        if (glfwGetKey(window, i) == GLFW_PRESS) {
            return i; // Return the key code immediately
        }
    }

    return -1; // No keys are pressed, skip and keep moving
}