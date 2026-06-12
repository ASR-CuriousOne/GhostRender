#include "cameraController.hpp"
#include <GLFW/glfw3.h>
#include <algorithm>
#include <iostream>

CameraController::CameraController(Ghost::GhostCamera &camera,
                                   WindowGLFW &window)
    : m_camera(camera), m_window(window) {}

void CameraController::update(float dt) {
    if (m_mode == Mode::FreeRoam) {
        updateFreeRoam(dt);
    } else {
        updateOrbit(dt);
    }
}

void CameraController::updateFreeRoam(float dt) {
    GLFWwindow *window = m_window.getWindow();

    double mouseX, mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        if (m_firstMouse) {
            m_lastMouseX = mouseX;
            m_lastMouseY = mouseY;
            m_firstMouse = false;
        }

        float xoffset = (mouseX - m_lastMouseX) * m_lookSpeed;
        float yoffset = (m_lastMouseY - mouseY) * m_lookSpeed;

        m_yaw -= xoffset;
        m_pitch += yoffset;

        m_pitch = std::clamp(m_pitch, -1.5f, 1.5f);
    } else {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        m_firstMouse = true;
    }

    m_lastMouseX = mouseX;
    m_lastMouseY = mouseY;

    glm::vec3 forward;
    forward.x = cos(m_yaw) * cos(m_pitch);
    forward.y = sin(m_yaw) * cos(m_pitch);
    forward.z = sin(m_pitch);
    forward = glm::normalize(forward);

    glm::vec3 worldUp = {0.f, 0.f, 1.f};
    glm::vec3 right = glm::normalize(glm::cross(forward, worldUp));
    glm::vec3 up = glm::normalize(glm::cross(right, forward));

    glm::vec3 moveDir{0.f};
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        moveDir += forward;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        moveDir -= forward;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        moveDir += right;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        moveDir -= right;
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        moveDir += up;
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        moveDir -= up;

    float currentSpeed = m_moveSpeed;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        currentSpeed *= 20.0f;
    }

    if (glm::dot(moveDir, moveDir) > std::numeric_limits<float>::epsilon()) {
        m_position += currentSpeed * dt * glm::normalize(moveDir);
    }

    m_camera.setViewDirection(m_position, forward, worldUp);
}

void CameraController::updateOrbit(float dt) {
    float scrollY = m_window.consumeScrollOffset();
    if (scrollY != 0.0f) {
        float zoomSpeed = 1.0f;
        m_radius -= scrollY * zoomSpeed;
        m_radius = std::clamp(m_radius, 2.0f, 1000.0f);
    }

    double mouseX, mouseY;
    glfwGetCursorPos(m_window.getWindow(), &mouseX, &mouseY);

    if (glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        if (m_firstMouse) {
            m_lastMouseX = mouseX;
            m_lastMouseY = mouseY;
            m_firstMouse = false;
        }

        float xoffset = (mouseX - m_lastMouseX) * m_lookSpeed;
        float yoffset = (m_lastMouseY - mouseY) * m_lookSpeed;

        m_yaw -= xoffset;
        m_pitch -= yoffset;
        m_pitch = std::clamp(m_pitch, -1.5f, 1.5f);
    } else {
        m_firstMouse = true;
    }
    m_lastMouseX = mouseX;
    m_lastMouseY = mouseY;

    m_position.x = m_target.x + m_radius * cos(m_pitch) * cos(m_yaw);
    m_position.y = m_target.y + m_radius * cos(m_pitch) * sin(m_yaw);
    m_position.z = m_target.z + m_radius * sin(m_pitch);

    glm::vec3 worldUp = {0.f, 0.f, 1.f};
    m_camera.setViewTarget(m_position, m_target, worldUp);
}
