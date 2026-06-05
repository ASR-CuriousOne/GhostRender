#pragma once
#include <Ghost/Resources/ghostModel.hpp>
#include <glm/glm.hpp>

namespace Ghost {
struct GhostRenderObject {
    glm::mat4 transformMatrix{1.f};
    std::shared_ptr<GhostModel> model{};

	vk::DescriptorSet textureDescriptorSet = nullptr;
};
} // namespace Ghost
