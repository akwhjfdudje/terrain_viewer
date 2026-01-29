#include "camera.h"

Camera::Camera(float fovDeg, float aspect, float nearZ, float farZ)
    : m_fov(fovDeg), m_aspect(aspect), m_near(nearZ), m_far(farZ) {
    updateVectors();
}

void Camera::setAspect(float aspect) {
    m_aspect = aspect;
}

void Camera::processKeyboard(float dx, float dy, float dz, float dt) {
    float v = m_speed * dt;
    m_pos += m_right * dx * v;
    m_pos += m_up    * dy * v;
    m_pos += m_front * dz * v;
}

void Camera::processMouse(float dx, float dy) {
    dx *= m_sensitivity;
    dy *= m_sensitivity;

    m_yaw   += dx;
    m_pitch += dy;

    if (m_pitch > 89.0f)  m_pitch = 89.0f;
    if (m_pitch < -89.0f) m_pitch = -89.0f;

    updateVectors();
}

void Camera::processScroll(float dy) {
    m_fov -= dy;
    if (m_fov < 20.0f) m_fov = 20.0f;
    if (m_fov > 90.0f) m_fov = 90.0f;
}

glm::mat4 Camera::view() const {
    return glm::lookAt(m_pos, m_pos + m_front, m_up);
}

glm::mat4 Camera::projection() const {
    return glm::perspective(glm::radians(m_fov), m_aspect, m_near, m_far);
}

void Camera::updateVectors() {
    glm::vec3 front{
        cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch)),
        sin(glm::radians(m_pitch)),
        sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch))
    };
    m_front = glm::normalize(front);
    m_right = glm::normalize(glm::cross(m_front, glm::vec3(0,1,0)));
    m_up    = glm::normalize(glm::cross(m_right, m_front));
}

