#include <GLFW/glfw3.h>

void mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
    (void)window;

    // temp
    (void)xpos;
    (void)ypos;
    // TODO: add yaw and pitch
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}
