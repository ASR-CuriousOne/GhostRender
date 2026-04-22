#include <Ghost/ghostModel.hpp>

namespace Ghost {
std::vector<vk::VertexInputBindingDescription>
Vertex::getBindingDescriptions() {
    std::vector<vk::VertexInputBindingDescription> bindingDescriptions(1);
    bindingDescriptions[0]
        .setBinding(0)
        .setStride(sizeof(Vertex))
        .setInputRate(vk::VertexInputRate::eVertex);
    return bindingDescriptions;
}

std::vector<vk::VertexInputAttributeDescription>
Vertex::getAttributeDescriptions() {
    std::vector<vk::VertexInputAttributeDescription> attributeDescriptions(2);
    attributeDescriptions[0]
        .setBinding(0)
        .setLocation(0)
        .setFormat(vk::Format::eR32G32Sfloat)
        .setOffset(offsetof(Vertex, position));
    attributeDescriptions[1]
        .setBinding(0)
        .setLocation(1)
        .setFormat(vk::Format::eR32G32B32Sfloat)
        .setOffset(offsetof(Vertex, color));
    return attributeDescriptions;
}

GhostModel::GhostModel(const VulkanDevice &device,
                       const std::vector<Vertex> &vertices)
    : m_device(device), m_vertexCount(static_cast<uint32_t>(vertices.size())) {

    vk::DeviceSize bufferSize = sizeof(vertices[0]) * m_vertexCount;

    m_vertexBuffer = std::make_unique<GhostBuffer>(
        m_device, bufferSize, vk::BufferUsageFlagBits::eVertexBuffer,
        vk::MemoryPropertyFlagBits::eHostVisible |
            vk::MemoryPropertyFlagBits::eHostCoherent);

    m_vertexBuffer->writeToBuffer(std::span<const Vertex>(vertices));
}

GhostModel::~GhostModel() {}

void GhostModel::bind(const vk::raii::CommandBuffer &commandBuffer) {
    vk::Buffer buffers[] = {m_vertexBuffer->getBuffer()};
    vk::DeviceSize offsets[] = {0};
    commandBuffer.bindVertexBuffers(0, buffers, offsets);
}

void GhostModel::draw(const vk::raii::CommandBuffer &commandBuffer) {
    commandBuffer.draw(m_vertexCount, 1, 0, 0);
}

} // namespace Ghost
