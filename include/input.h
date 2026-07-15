#pragma once

#include <GLFW/glfw3.h>
#include "camera.hpp"

void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
int glfw_getch_nonblocking(GLFWwindow* window);

extern Camera* globalCamPtr;