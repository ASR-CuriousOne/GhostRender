#pragma once
#include <Ghost/ghostTexture.hpp>
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <Ghost/ghostModel.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Ghost {

struct TransformComponent {
    glm::vec3 translation{};
    glm::vec3 scale{1.f, 1.f, 1.f};
    glm::vec3 rotation;

    glm::mat4 mat4() const {
        auto transform = glm::translate(glm::mat4{1.f}, translation);
        transform = glm::rotate(transform, rotation.y, {0.f, 1.f, 0.f});
        transform = glm::rotate(transform, rotation.x, {1.f, 0.f, 0.f});
        transform = glm::rotate(transform, rotation.z, {0.f, 0.f, 1.f});
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

    std::shared_ptr<GhostModel> model{};
    std::shared_ptr<GhostTexture> texture{};
    vk::raii::DescriptorSet textureDescriptorSet = nullptr;
    TransformComponent transform{};

  private:
    GhostGameObject(id_t objId) : m_id{objId} {}
    id_t m_id;
};

} // namespace Ghost
