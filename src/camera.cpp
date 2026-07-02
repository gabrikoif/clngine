#include "camera.h"

// Constructor: Uses initializer list to set up default values
Camera::Camera(glm::vec3 startPos)
    : cameraPos(startPos),
      viewDir(0.0f, 0.0f, -1.0f),
      yaw(-90.0f), // -90 degrees faces down the negative Z-axis (standard OpenGL)
      pitch(0.0f)
{
}

// Generates the View Matrix based on current position and look direction
glm::mat4 Camera::genView() const
{
    return glm::lookAt(cameraPos, cameraPos + viewDir, glm::vec3(0.0f, 1.0f, 0.0f));
}

// Generates the Projection Matrix (Perspective)
glm::mat4 Camera::genProj(float aspectRatio) const
{
    return glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);
    // near and far stand for render distance close and far away.
}

// Call this from your external mouse input handler to update angles and viewDir
void Camera::handleMouseMovement(float xOffset, float yOffset)
{
    float sensitivity = 0.1f;
    xOffset *= sensitivity;
    yOffset *= sensitivity;

    yaw += xOffset;
    pitch += yOffset;

    // Constrain pitch so the camera doesn't flip upside down
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    // Direct 3D math: Calculate the new direction vector from spherical coordinates
    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

    // Normalize it so it stays exactly 1 unit long
    viewDir = glm::normalize(direction);
}