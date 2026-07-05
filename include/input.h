#pragma once

#include <GLFW/glfw3.h>
#include "camera.hpp"

void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void handle_input(GLFWwindow *window);

extern Camera *globalCamPointer;