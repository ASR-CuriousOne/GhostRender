#include <Ghost/ghostGraphicsPipeline.hpp>
#include <Ghost/utils.hpp>
#include <vulkan/vulkan_raii.hpp>

namespace Ghost {
GhostGraphicsPipeline::GhostGraphicsPipeline(
    const VulkanDevice &device, const std::filesystem::path &vertFilepath,
    const std::filesystem::path &fragFilepath,
    const PipelineConfigInfo &configInfo)
    : m_device(device) {

    const std::vector<std::byte> vertShaderCode = Utils::readFile(vertFilepath);
    const std::vector<std::byte> fragShaderCode = Utils::readFile(fragFilepath);

    vk::ShaderModuleCreateInfo vertCreateInfo(
        {}, vertShaderCode.size(),
        reinterpret_cast<const uint32_t *>(vertShaderCode.data()));
    m_vertShaderModule = vk::raii::ShaderModule(m_device, vertCreateInfo);

    vk::ShaderModuleCreateInfo fragCreateInfo(
        {}, fragShaderCode.size(),
        reinterpret_cast<const uint32_t *>(fragShaderCode.data()));
    m_fragShaderModule = vk::raii::ShaderModule(m_device, fragCreateInfo);

    vk::PipelineShaderStageCreateInfo shaderStages[2] = {
        {{}, vk::ShaderStageFlagBits::eVertex, *m_vertShaderModule, "main"},
        {{}, vk::ShaderStageFlagBits::eFragment, *m_fragShaderModule, "main"}};

    auto createInfo = vk::GraphicsPipelineCreateInfo()
                          .setStages(shaderStages)
                          .setPVertexInputState(&configInfo.vertexInputInfo)
                          .setPInputAssemblyState(&configInfo.inputAssemblyInfo)
                          .setPViewportState(&configInfo.viewportInfo)
                          .setPRasterizationState(&configInfo.rasterizationInfo)
                          .setPMultisampleState(&configInfo.multisampleInfo)
                          .setPDepthStencilState(&configInfo.depthStencilInfo)
                          .setPColorBlendState(&configInfo.colorBlendInfo)
                          .setPDynamicState(&configInfo.dynamicStateCreateInfo)
                          .setLayout(configInfo.pipelineLayout)
                          .setRenderPass(configInfo.renderPass)
                          .setSubpass(configInfo.subpass)
                          .setBasePipelineHandle(nullptr)
                          .setBasePipelineIndex(-1);

    m_graphicsPipeline = vk::raii::Pipeline(m_device, nullptr, createInfo);
}

GhostGraphicsPipeline::~GhostGraphicsPipeline(){}
} // namespace Ghost
