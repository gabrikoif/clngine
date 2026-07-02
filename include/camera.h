#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera
{
public:
    // Constructor with default argument
    Camera(glm::vec3 startPos = glm::vec3(0.0f, 0.0f, 0.0f));

    glm::vec3 cameraPos;
    glm::vec3 viewDir;

    float fov;
    float nearPlane;
    float farPlane;

    // const means these functions promise not to alter cameraPos, yaw, pitch, etc.
    glm::mat4 genView() const;
    glm::mat4 genProj(float aspectRatio) const; 

    void handleMouseMovement(float xOffset, float yOffset);

private:
    float yaw;
    float pitch;
};