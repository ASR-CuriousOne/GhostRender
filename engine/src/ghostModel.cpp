#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#define TINYOBJLOADER_IMPLEMENTATION
#include <vendor/tiny_obj_loader.h>

#include <Ghost/ghostModel.hpp>

#include <iostream>
#include <unordered_map>

namespace std {
template <> struct hash<Ghost::Vertex> {
    size_t operator()(Ghost::Vertex const &vertex) const {
        size_t seed = 0;
        seed ^= hash<glm::vec3>()(vertex.position) + 0x9e3779b9 + (seed << 6) +
                (seed >> 2);
        seed ^= hash<glm::vec3>()(vertex.color) + 0x9e3779b9 + (seed << 6) +
                (seed >> 2);
        seed ^= hash<glm::vec2>()(vertex.uv) + 0x9e3779b9 + (seed << 6) +
                (seed >> 2);
        seed ^= hash<glm::vec3>()(vertex.normal) + 0x9e3779b9 + (seed << 6) +
                (seed >> 2);
        return seed;
    }
};
} // namespace std

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
    std::vector<vk::VertexInputAttributeDescription> attributeDescriptions(4);
    attributeDescriptions[0]
        .setBinding(0)
        .setLocation(0)
        .setFormat(vk::Format::eR32G32B32Sfloat)
        .setOffset(offsetof(Vertex, position));
    attributeDescriptions[1]
        .setBinding(0)
        .setLocation(1)
        .setFormat(vk::Format::eR32G32B32Sfloat)
        .setOffset(offsetof(Vertex, color));
    attributeDescriptions[2]
        .setBinding(0)
        .setLocation(2)
        .setFormat(vk::Format::eR32G32Sfloat)
        .setOffset(offsetof(Vertex, uv));
    attributeDescriptions[3]
        .setBinding(0)
        .setLocation(3)
        .setFormat(vk::Format::eR32G32B32Sfloat)
        .setOffset(offsetof(Vertex, normal));

    return attributeDescriptions;
}

GhostModel::GhostModel(VulkanDevice &device,
                       const std::vector<Vertex> &vertices,
                       const std::vector<uint32_t> &indicies)
    : m_device(device), m_vertexCount(static_cast<uint32_t>(vertices.size())),
      m_indexCount(static_cast<uint32_t>(indicies.size())) {

    vk::DeviceSize bufferSize = sizeof(vertices[0]) * m_vertexCount;

    GhostBuffer stagingVertexBuffer(
        m_device, bufferSize, vk::BufferUsageFlagBits::eTransferSrc,
        vk::MemoryPropertyFlagBits::eHostVisible |
            vk::MemoryPropertyFlagBits::eHostCoherent);

    stagingVertexBuffer.writeToBuffer(std::span<const Vertex>(vertices));

    m_vertexBuffer = std::make_unique<GhostBuffer>(
        m_device, bufferSize,
        vk::BufferUsageFlagBits::eTransferDst |
            vk::BufferUsageFlagBits::eVertexBuffer,
        vk::MemoryPropertyFlagBits::eDeviceLocal);

    auto commandBuffer = m_device.get().beginSingleTimeCommands();

    vk::BufferCopy copyRegion;
    copyRegion.setSrcOffset(0).setDstOffset(0).setSize(bufferSize);
    commandBuffer.copyBuffer(stagingVertexBuffer.getBuffer(),
                             m_vertexBuffer->getBuffer(), copyRegion);

    m_device.get().endSingleTimeCommands(commandBuffer);

    vk::DeviceSize indexBufferSize = sizeof(uint32_t) * m_indexCount;
    GhostBuffer stagingIndexBuffer(
        m_device, indexBufferSize, vk::BufferUsageFlagBits::eTransferSrc,
        vk::MemoryPropertyFlagBits::eHostVisible |
            vk::MemoryPropertyFlagBits::eHostCoherent);

    stagingIndexBuffer.writeToBuffer(std::span<const uint32_t>(indicies));

    m_indexBuffer =
        std::make_unique<GhostBuffer>(m_device, indexBufferSize,
                                      vk::BufferUsageFlagBits::eTransferDst |
                                          vk::BufferUsageFlagBits::eIndexBuffer,
                                      vk::MemoryPropertyFlagBits::eDeviceLocal);

    commandBuffer = m_device.get().beginSingleTimeCommands();

    vk::BufferCopy indexCopyRegion;
    indexCopyRegion.setSrcOffset(0).setDstOffset(0).setSize(indexBufferSize);
    commandBuffer.copyBuffer(stagingIndexBuffer.getBuffer(),
                             m_indexBuffer->getBuffer(), indexCopyRegion);

    m_device.get().endSingleTimeCommands(commandBuffer);
}

GhostModel::~GhostModel() {}

void GhostModel::Builder::loadModel(const std::string &filepath) {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err,
                          filepath.c_str())) {
        throw std::runtime_error(warn + err);
    }

    vertices.clear();
    indices.clear();

    std::unordered_map<Vertex, uint32_t> uniqueVertices{};

    for (const auto &shape : shapes) {
        for (const auto &index : shape.mesh.indices) {
            Vertex vertex{};

            vertex.position = {attrib.vertices[3 * index.vertex_index + 0],
                               attrib.vertices[3 * index.vertex_index + 1],
                               attrib.vertices[3 * index.vertex_index + 2]};

            if (!attrib.colors.empty()) {
                vertex.color = {attrib.colors[3 * index.vertex_index + 0],
                                attrib.colors[3 * index.vertex_index + 1],
                                attrib.colors[3 * index.vertex_index + 2]};
            } else {
                vertex.color = {1.0f, 1.0f, 1.0f};
            }

            if (index.texcoord_index >= 0) {
                vertex.uv = {
                    attrib.texcoords[2 * index.texcoord_index + 0],
                    1.0f - attrib.texcoords[2 * index.texcoord_index + 1]};
            }

            vertex.normal = {attrib.normals[3 * index.normal_index + 0],
                             attrib.normals[3 * index.normal_index + 1],
                             attrib.normals[3 * index.normal_index + 2]};

            if (uniqueVertices.count(vertex) == 0) {
                uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
                vertices.push_back(vertex);
            }
            indices.push_back(uniqueVertices[vertex]);
        }
    }
}

std::shared_ptr<GhostModel>
GhostModel::createModelFromFile(VulkanDevice &device,
                                const std::string &filepath) {
    Builder builder{};
    builder.loadModel(filepath);

    std::clog << "Loaded Model: " << filepath
              << " | Vertices: " << builder.vertices.size()
              << " | Indices: " << builder.indices.size() << std::endl;

    return std::make_shared<GhostModel>(device, builder.vertices,
                                        builder.indices);
}

void GhostModel::bind(const vk::raii::CommandBuffer &commandBuffer) {
    vk::Buffer vertexBuffers[] = {m_vertexBuffer->getBuffer()};
    vk::Buffer indexBuffer = m_indexBuffer->getBuffer();
    vk::DeviceSize offsets[] = {0};
    commandBuffer.bindVertexBuffers(0, vertexBuffers, offsets);
    commandBuffer.bindIndexBuffer(indexBuffer, 0, vk::IndexType::eUint32);
}

void GhostModel::draw(const vk::raii::CommandBuffer &commandBuffer) {
    commandBuffer.drawIndexed(static_cast<uint32_t>(m_indexCount), 1, 0, 0, 0);
}

} // namespace Ghost
