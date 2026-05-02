#pragma once
#include <Ghost/ghostBuffer.hpp>
#include <functional>
#include <glm/glm.hpp>

namespace Ghost {
struct Vertex {
    glm::vec3 position;
    glm::vec3 color;
    glm::vec2 uv;

    static std::vector<vk::VertexInputBindingDescription>
    getBindingDescriptions();
    static std::vector<vk::VertexInputAttributeDescription>
    getAttributeDescriptions();

    bool operator==(const Vertex &other) const {
        return position == other.position && color == other.color &&
               uv == other.uv;
    }
};

class GhostModel {
  public:
    struct Builder {
        std::vector<Vertex> vertices{};
        std::vector<uint32_t> indices{};

        void loadModel(const std::string &filepath);
    };

    GhostModel(VulkanDevice &device, const std::vector<Vertex> &vertices,
               const std::vector<uint32_t> &indicies);
    ~GhostModel();

    GhostModel(const GhostModel &) = delete;
    GhostModel &operator=(const GhostModel &) = delete;

    static std::shared_ptr<GhostModel>
    createModelFromFile(VulkanDevice &device, const std::string &filepath);

    void bind(const vk::raii::CommandBuffer &commandBuffer);
    void draw(const vk::raii::CommandBuffer &commandBuffer);

  private:
    std::reference_wrapper<VulkanDevice> m_device;
    std::unique_ptr<GhostBuffer> m_vertexBuffer;
    std::unique_ptr<GhostBuffer> m_indexBuffer;
    uint32_t m_vertexCount;
    uint32_t m_indexCount;
};
} // namespace Ghost
