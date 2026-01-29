#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:
    Camera(float fovDeg, float aspect, float nearZ, float farZ);

    void setAspect(float aspect);

    void processKeyboard(float dx, float dy, float dz, float dt);
    void processMouse(float dx, float dy);
    void processScroll(float dy);

    glm::mat4 view() const;
    glm::mat4 projection() const;

    const glm::vec3& position() const { return m_pos; }

private:
    void updateVectors();

private:
    glm::vec3 m_pos{0.0f, 1.5f, 3.0f};
    glm::vec3 m_front{0.0f, 0.0f, -1.0f};
    glm::vec3 m_up{0.0f, 1.0f, 0.0f};
    glm::vec3 m_right{1.0f, 0.0f, 0.0f};

    float m_yaw   = -90.0f;
    float m_pitch = 0.0f;

    float m_fov;
    float m_aspect;
    float m_near;
    float m_far;

    float m_speed = 5.0f;
    float m_sensitivity = 0.1f;
};

