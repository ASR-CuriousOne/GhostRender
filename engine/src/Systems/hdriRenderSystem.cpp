#include "Ghost/Resources/mesh.hpp"
#include <Ghost/Systems/hdriRenderSystem.hpp>
#include <Ghost/Utils/utils.hpp>
#include <cstddef>

namespace Ghost {

HDRIRenderSystem::HDRIRenderSystem(VulkanDevice &device,
                                   vk::RenderPass renderPass,
                                   vk::DescriptorSetLayout globalSetLayout,
                                   vk::DescriptorSetLayout hdriSetLayout)
    : m_device(device) {
    createPipelineLayout(globalSetLayout, hdriSetLayout);
    createPipeline(renderPass);
    loadCubeModel();
}

void HDRIRenderSystem::createPipeline(vk::RenderPass renderPass) {
    auto envVars = Utils::loadEnvFile(".env");
    auto skyboxVertShaderPath = envVars.contains("SKYBOX_VERT_SHADER_PATH")
                                    ? envVars["SKYBOX_VERT_SHADER_PATH"]
                                    : "./shaders/skyboxVert.spv";
    auto skyboxFragShaderPath = envVars.contains("SKYBOX_FRAG_SHADER_PATH")
                                    ? envVars["SKYBOX_FRAG_SHADER_PATH"]
                                    : "./shaders/skyboxFrag.spv";

    const std::vector<std::byte> vertShaderCode =
        Utils::readFile(skyboxVertShaderPath);
    const std::vector<std::byte> fragShaderCode =
        Utils::readFile(skyboxFragShaderPath);

    PipelineConfigInfo pipelineConfigInfo;
    PipelineConfigInfo::defaultConfig(pipelineConfigInfo);
    pipelineConfigInfo.renderPass = renderPass;
    pipelineConfigInfo.pipelineLayout = m_pipelineLayout;

    pipelineConfigInfo.bindingDescriptions =
        StandardVertex::getBindingDescriptions();
    pipelineConfigInfo.attributeDescriptions =
        StandardVertex::getAttributeDescriptions();
    pipelineConfigInfo.vertexInputInfo
        .setVertexBindingDescriptions(pipelineConfigInfo.bindingDescriptions)
        .setVertexAttributeDescriptions(
            pipelineConfigInfo.attributeDescriptions);
    pipelineConfigInfo.depthStencilInfo.depthWriteEnable = VK_FALSE;
    pipelineConfigInfo.depthStencilInfo.depthCompareOp =
        vk::CompareOp::eLessOrEqual;

    pipelineConfigInfo.rasterizationInfo.setCullMode(
        vk::CullModeFlagBits::eFront);

    m_graphicsPipeline = std::make_unique<GhostGraphicsPipeline>(
        m_device, vertShaderCode, fragShaderCode, pipelineConfigInfo);
}

void HDRIRenderSystem::createPipelineLayout(
    vk::DescriptorSetLayout globalSetLayout,
    vk::DescriptorSetLayout hdriSetLayout) {
    std::vector<vk::DescriptorSetLayout> descriptorSetLayouts = {
        globalSetLayout, hdriSetLayout};

    vk::PipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.setSetLayouts(descriptorSetLayouts);

    m_pipelineLayout = vk::raii::PipelineLayout(m_device, pipelineLayoutInfo);
}

void HDRIRenderSystem::render(
    const FrameInfo &frameInfo, std::vector<GhostRenderObject> &renderObjects,
    const vk::raii::DescriptorSet &globalDescriptorSet) {

    frameInfo.commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics,
                                         **m_graphicsPipeline);

    std::vector<vk::DescriptorSet> descriptorSets = {*globalDescriptorSet,
                                                     *m_hdriDescriptorSet};

    frameInfo.commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
                                               *m_pipelineLayout, 0,
                                               descriptorSets, nullptr);

    if (m_cubeModel) {
        m_cubeModel->bind(frameInfo.commandBuffer);
        m_cubeModel->draw(frameInfo.commandBuffer);
    }
}

void HDRIRenderSystem::loadCubeModel() {
    glm::vec3 c = {1.0f, 1.0f, 1.0f};
    glm::vec2 u = {0.0f, 0.0f};
    glm::vec3 n = {0.0f, 0.0f, 0.0f};

    std::vector<StandardVertex> vertices = {
        {{-1.0f, -1.0f, -1.0f}, c, u, n}, {{1.0f, -1.0f, -1.0f}, c, u, n},
        {{1.0f, 1.0f, -1.0f}, c, u, n},   {{-1.0f, 1.0f, -1.0f}, c, u, n},
        {{-1.0f, -1.0f, 1.0f}, c, u, n},  {{1.0f, -1.0f, 1.0f}, c, u, n},
        {{1.0f, 1.0f, 1.0f}, c, u, n},    {{-1.0f, 1.0f, 1.0f}, c, u, n}};

    std::vector<uint32_t> indices = {0, 2, 1, 0, 3, 2, 4, 5, 6, 4, 6, 7,
                                     4, 7, 3, 4, 3, 0, 1, 2, 6, 1, 6, 5,
                                     4, 0, 1, 4, 1, 5, 3, 7, 6, 3, 6, 2};

    std::span<StandardVertex> vertexSpan(vertices);
    std::span<StandardVertex> indexSpan(vertices);

    m_cubeModel =
        std::make_unique<Mesh>(m_device, std::as_bytes(vertexSpan),
                               vertices.size(), indices, VertexType::Standard);
}
} // namespace Ghost
