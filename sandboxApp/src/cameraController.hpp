#pragma once

#include "ghostCamera.hpp"
#include "windowGLFW.hpp"
#include <glm/glm.hpp>

class CameraController {
  public:
    enum class Mode { FreeRoam, Orbit };

    CameraController(Ghost::GhostCamera &camera, WindowGLFW &window);

    void update(float dt);

    void setMode(Mode mode) {
        m_mode = mode;
        m_firstMouse = true;
    }
    void setTarget(const glm::vec3 &target) { m_target = target; }
    void setRadius(float radius) { m_radius = radius; }

    Mode getMode() const { return m_mode; }

  private:
    void updateFreeRoam(float dt);
    void updateOrbit(float dt);

    Ghost::GhostCamera &m_camera;
    WindowGLFW &m_window;

    Mode m_mode = Mode::FreeRoam;

    glm::vec3 m_position{0.f, 0.f, -5.f};
    glm::vec3 m_target{0.f, 0.f, 0.f};
    float m_yaw = 0.f;
    float m_pitch = 0.f;
    float m_radius = 20.f;

    double m_lastMouseX = 0.0;
    double m_lastMouseY = 0.0;
    bool m_firstMouse = true;

    float m_moveSpeed = 5.f;
    float m_lookSpeed = 0.005f;
};
