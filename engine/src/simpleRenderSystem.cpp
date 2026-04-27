#include <Ghost/simpleRenderSystem.hpp>
#include <Ghost/utils.hpp>

namespace Ghost {
struct PushConstantData {
    glm::mat4 model{1.f};
};

SimpleRenderSystem::SimpleRenderSystem(VulkanDevice &device,
                                       vk::RenderPass renderPass,
                                       vk::DescriptorSetLayout globalSetLayout)
    : m_device(device) {
    createPipelineLayout(globalSetLayout);
    createPipeline(renderPass);
}

SimpleRenderSystem::~SimpleRenderSystem() {}

void SimpleRenderSystem::createPipelineLayout(
    vk::DescriptorSetLayout globalSetLayout) {
    vk::PushConstantRange pushConstantRange;
    pushConstantRange.setStageFlags(vk::ShaderStageFlagBits::eVertex)
        .setOffset(0)
        .setSize(sizeof(PushConstantData));

    vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo;
    pipelineLayoutCreateInfo.setSetLayoutCount(1)
        .setSetLayouts(globalSetLayout)
        .setPushConstantRangeCount(1)
        .setPushConstantRanges(pushConstantRange);

    m_pipelineLayout =
        vk::raii::PipelineLayout(m_device, pipelineLayoutCreateInfo);
}

void SimpleRenderSystem::createPipeline(vk::RenderPass renderPass) {
    auto envVars = Utils::loadEnvFile(".env");
    auto vertShaderPath = envVars.contains("VERT_SHADER_PATH")
                              ? envVars["VERT_SHADER_PATH"]
                              : "./shaders/vert.spv";
    auto fragShaderPath = envVars.contains("FRAG_SHADER_PATH")
                              ? envVars["FRAG_SHADER_PATH"]
                              : "./shaders/frag.spv";

    PipelineConfigInfo pipelineConfigInfo;
    PipelineConfigInfo::defaultConfig(pipelineConfigInfo);
    pipelineConfigInfo.renderPass = renderPass;
    pipelineConfigInfo.pipelineLayout = m_pipelineLayout;

    pipelineConfigInfo.bindingDescriptions = Vertex::getBindingDescriptions();
    pipelineConfigInfo.attributeDescriptions =
        Vertex::getAttributeDescriptions();
    pipelineConfigInfo.vertexInputInfo
        .setVertexBindingDescriptions(pipelineConfigInfo.bindingDescriptions)
        .setVertexAttributeDescriptions(
            pipelineConfigInfo.attributeDescriptions);

    m_graphicsPipeline = std::make_unique<GhostGraphicsPipeline>(
        m_device, vertShaderPath, fragShaderPath, pipelineConfigInfo);
}

void SimpleRenderSystem::renderGameObjects(
    const vk::raii::CommandBuffer &commandBuffer,
    std::vector<GhostGameObject> &gameObjects,
    const vk::raii::DescriptorSet &globalDescriptorSet) {

    commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics,
                               **m_graphicsPipeline);

    commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
                                     *m_pipelineLayout, 0,
                                     {*globalDescriptorSet}, nullptr);

    for (auto &obj : gameObjects) {
        PushConstantData push{};
        push.model = obj.transform.mat4();

        commandBuffer.pushConstants<PushConstantData>(
            *m_pipelineLayout, vk::ShaderStageFlagBits::eVertex, 0, push);

        obj.model->bind(commandBuffer);
        obj.model->draw(commandBuffer);
    }
}

} // namespace Ghost
