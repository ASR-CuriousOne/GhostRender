#include <Ghost/ghostCamera.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

namespace Ghost {
void GhostCamera::setOrthographicProjection(float left, float right, float top,
                                            float bottom, float near,
                                            float far) {
    m_projectionMatrix = glm::ortho(left, right, bottom, top, near, far);
    m_projectionMatrix[1][1] *= -1;
}

void GhostCamera::setPerspectiveProjection(float fovy, float aspect, float near,
                                           float far) {
    assert(glm::abs(aspect - std::numeric_limits<float>::epsilon()) > 0.0f);
    m_projectionMatrix = glm::perspective(fovy, aspect, near, far);
    m_projectionMatrix[1][1] *= -1;
}

void GhostCamera::setViewDirection(glm::vec3 position, glm::vec3 direction,
                                   glm::vec3 up) {
    m_viewMatrix = glm::lookAt(position, position + direction, up);
}

void GhostCamera::setViewTarget(glm::vec3 position, glm::vec3 target,
                                glm::vec3 up) {
    m_viewMatrix = glm::lookAt(position, target, up);
}

void GhostCamera::setViewYXZ(glm::vec3 position, glm::vec3 rotation) {
    const float c3 = glm::cos(rotation.z);
    const float s3 = glm::sin(rotation.z);
    const float c2 = glm::cos(rotation.x);
    const float s2 = glm::sin(rotation.x);
    const float c1 = glm::cos(rotation.y);
    const float s1 = glm::sin(rotation.y);
    const glm::vec3 u{(c1 * c3 + s1 * s2 * s3), (c2 * s3),
                      (c1 * s2 * s3 - c3 * s1)};
    const glm::vec3 v{(c3 * s1 * s2 - c1 * s3), (c2 * c3),
                      (c1 * c3 * s2 + s1 * s3)};
    const glm::vec3 w{(c2 * s1), (-s2), (c1 * c2)};
    m_viewMatrix = glm::mat4{1.f};
    m_viewMatrix[0][0] = u.x;
    m_viewMatrix[1][0] = u.y;
    m_viewMatrix[2][0] = u.z;
    m_viewMatrix[0][1] = v.x;
    m_viewMatrix[1][1] = v.y;
    m_viewMatrix[2][1] = v.z;
    m_viewMatrix[0][2] = w.x;
    m_viewMatrix[1][2] = w.y;
    m_viewMatrix[2][2] = w.z;
    m_viewMatrix[3][0] = -glm::dot(u, position);
    m_viewMatrix[3][1] = -glm::dot(v, position);
    m_viewMatrix[3][2] = -glm::dot(w, position);
}

void GhostCamera::update(float dt) {
    m_accumulatedTime += dt;

    glm::vec3 cameraPos =
        glm::rotate(glm::mat4(1.0f), m_accumulatedTime * glm::radians(90.0f),
                    glm::vec3(0.0f, 0.0f, 1.0f)) *
        glm::vec4(1.0f, 1.0f, 3.0f, 1.0f);

    setViewTarget(cameraPos, glm::vec3(0.0f, 0.0f, 0.0f),
                  glm::vec3(0.0f, 0.0f, 1.0f));
}

} // namespace Ghost
