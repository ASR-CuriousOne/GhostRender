#include <Ghost/simpleRenderSystem.hpp>
#include <Ghost/utils.hpp>

namespace Ghost {
struct SimplePushConstantData {
    glm::mat4 modelMatrix{1.f};
    glm::mat4 normalMatrix{1.f};
};

SimpleRenderSystem::SimpleRenderSystem(VulkanDevice &device,
                                       vk::RenderPass renderPass,
                                       vk::DescriptorSetLayout globalSetLayout,
                                       vk::DescriptorSetLayout textureSetLayout)
    : m_device(device) {
    createPipelineLayout(globalSetLayout, textureSetLayout);
    createPipeline(renderPass);
}

SimpleRenderSystem::~SimpleRenderSystem() {}

void SimpleRenderSystem::createPipelineLayout(
    vk::DescriptorSetLayout globalSetLayout,
    vk::DescriptorSetLayout textureSetLayout) {
    vk::PushConstantRange pushConstantRange;
    pushConstantRange
        .setStageFlags(vk::ShaderStageFlagBits::eVertex |
                       vk::ShaderStageFlagBits::eFragment)
        .setOffset(0)
        .setSize(sizeof(SimplePushConstantData));

    std::vector<vk::DescriptorSetLayout> setLayouts = {globalSetLayout,
                                                       textureSetLayout};

    vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo;
    pipelineLayoutCreateInfo
        .setSetLayoutCount(static_cast<uint32_t>(setLayouts.size()))
        .setPSetLayouts(setLayouts.data())
        .setPushConstantRangeCount(1)
        .setPPushConstantRanges(&pushConstantRange);

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

void SimpleRenderSystem::render(
    const FrameInfo &frameInfo, std::vector<GhostRenderObject> &renderObjects,
    const vk::raii::DescriptorSet &globalDescriptorSet) {

    frameInfo.commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics,
                                         **m_graphicsPipeline);

    frameInfo.commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
                                               *m_pipelineLayout, 0,
                                               {*globalDescriptorSet}, nullptr);

    for (auto &obj : renderObjects) {
        SimplePushConstantData push{};
        push.modelMatrix = obj.transformMatrix;
        push.normalMatrix = glm::transpose(glm::inverse(push.modelMatrix));

        frameInfo.commandBuffer.pushConstants<SimplePushConstantData>(
            *m_pipelineLayout,
            vk::ShaderStageFlagBits::eVertex |
                vk::ShaderStageFlagBits::eFragment,
            0, push);

        if (obj.textureDescriptorSet) {
            frameInfo.commandBuffer.bindDescriptorSets(
                vk::PipelineBindPoint::eGraphics, *m_pipelineLayout, 1,
                {obj.textureDescriptorSet}, nullptr);
        }

        obj.model->bind(frameInfo.commandBuffer);
        obj.model->draw(frameInfo.commandBuffer);
    }
}

} // namespace Ghost
