#include "camera.hpp"

// Constructor: Uses initializer list to set up default values
Camera::Camera(
    float p_speed,
    glm::vec3 p_pos,
    glm::vec3 p_dir,
    float p_fov,
    float p_nearPlane,
    float p_farPlane,
    bool p_fly)
    : m_pos(p_pos),
      m_forward(p_dir),
      m_fov(p_fov),
      m_nearPlane(p_nearPlane),
      m_farPlane(p_farPlane),
      m_speed(p_speed),
      m_fly(p_fly)
{
    m_forward = glm::normalize(p_dir);
    m_right = glm::normalize(glm::cross(m_forward, glUp));

    m_move_forward = m_forward;
}

// Generates the View Matrix based on current position and look direction
glm::mat4 Camera::genView() const
{
    return glm::lookAt(m_pos, m_pos + m_forward, glUp);
}

// Generates the Projection Matrix (Perspective)
glm::mat4 Camera::genProj(float aspectRatio) const
{
    return glm::perspective(glm::radians(m_fov), aspectRatio, m_nearPlane, m_farPlane);
    // near and far stand for render distance close and far away.
}

void Camera::handleMouseMovement(float xOffset, float yOffset)
{
    float sensitivity = 0.1f;
    yaw += xOffset * sensitivity;
    pitch += yOffset * sensitivity;

    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    m_forward = glm::normalize(front);

    m_right = glm::normalize(glm::cross(m_forward, glUp));

    if (m_fly)
    {
        m_move_forward = m_forward;
    }
    else
    {
        m_move_forward = glm::normalize(glm::vec3(m_forward.x, 0.0f, m_forward.z));
    }
}

void Camera::handleKeyboard(bool w, bool a, bool s, bool d, float deltaTime)
{
    if (w)
        m_pos += m_move_forward * m_speed * deltaTime;
    if (s)
        m_pos -= m_move_forward * m_speed * deltaTime;
    if (d)
        m_pos += m_right * m_speed * deltaTime;
    if (a)
        m_pos -= m_right * m_speed * deltaTime;
}