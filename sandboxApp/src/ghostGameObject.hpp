#pragma once
#include <Ghost/Resources/ghostTexture.hpp>
#include <Ghost/Resources/material.hpp>
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <Ghost/Resources/mesh.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace Ghost {

struct TransformComponent {
    glm::vec3 translation{};
    glm::vec3 scale{1.f, 1.f, 1.f};
    glm::quat rotation{1.f, 0.f, 0.f, 0.f};

    glm::mat4 mat4() const {
        auto transform = glm::translate(glm::mat4{1.f}, translation);
        transform *= glm::mat4_cast(rotation);
        transform = glm::scale(transform, scale);
        return transform;
    }
};

class GhostGameObject {
  public:
    using id_t = unsigned int;

    static GhostGameObject createGameObject() {
        static id_t currentId = 0;
        return GhostGameObject{currentId++};
    }

    GhostGameObject(const GhostGameObject &) = delete;
    GhostGameObject &operator=(const GhostGameObject &) = delete;
    GhostGameObject(GhostGameObject &&) = default;
    GhostGameObject &operator=(GhostGameObject &&) = default;

    id_t getId() const { return m_id; }

    std::shared_ptr<Mesh> mesh{};
    std::shared_ptr<Material> material{};
    TransformComponent transform{};

    void update(float deltaTime);

  private:
    GhostGameObject(id_t objId) : m_id{objId} {}
    id_t m_id;

    float m_accumulatedTime = 0.0f;
};

} // namespace Ghost
