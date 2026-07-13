#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera
{
public:
    // Constructor with default argument
    Camera(
        glm::vec3 p_pos = glm::vec3(0.0f),
        glm::vec3 p_dir = glm::vec3(0.0f, 0.0f, -1.0f),
        float p_fov = 100.0f,
        float p_nearPlane = 0.1f,
        float p_farPlane = 1000.0f,
        float p_speed = 4.0f,
        bool p_fly = true);

    glm::vec3 m_pos;
    bool m_fly;

    // const means these functions promise not to alter cameraPos, yaw, pitch, etc.
    glm::mat4 genView() const;
    glm::mat4 genProj(float aspectRatio) const;

    void handleMouseMovement(float xOffset, float yOffset);
    void handleKeyboard(bool w, bool a, bool s, bool d, float DeltaTime = 0.016f);

private:
    float yaw;
    float pitch;
    float m_fov;
    float m_nearPlane;
    float m_farPlane;
    float m_speed;

    glm::vec3 m_forward;
    glm::vec3 m_right;

    glm::vec3 m_move_forward;
};

inline constexpr glm::vec3 glUp = glm::vec3(0.0f, 1.0f, 0.0f);