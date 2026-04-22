#pragma once
#include <Ghost/ghostBuffer.hpp>
#include <glm/glm.hpp>

namespace Ghost {
struct Vertex {
    glm::vec2 position;
    glm::vec3 color;

    static std::vector<vk::VertexInputBindingDescription>
    getBindingDescriptions();
    static std::vector<vk::VertexInputAttributeDescription>
    getAttributeDescriptions();
};

class GhostModel {
  public:
    GhostModel(VulkanDevice &device, const std::vector<Vertex> &vertices);
    ~GhostModel();

    GhostModel(const GhostModel &) = delete;
    GhostModel &operator=(const GhostModel &) = delete;

    void bind(const vk::raii::CommandBuffer &commandBuffer);
    void draw(const vk::raii::CommandBuffer &commandBuffer);

  private:
    const VulkanDevice &m_device;
    std::unique_ptr<GhostBuffer> m_vertexBuffer;
    uint32_t m_vertexCount;
};
} // namespace Ghost
