#pragma once
#include <Ghost/pipelineConfig.hpp>
#include <Ghost/vulkanDevice.hpp>
#include <filesystem>

namespace Ghost {

class GhostGraphicsPipeline {

    const VulkanDevice &m_device;

    vk::raii::Pipeline m_graphicsPipeline = nullptr;
    vk::raii::ShaderModule m_vertShaderModule = nullptr;
    vk::raii::ShaderModule m_fragShaderModule = nullptr;

  public:
    GhostGraphicsPipeline(const VulkanDevice &device,
                          const std::filesystem::path &vertFilepath,
                          const std::filesystem::path &fragFilepath,
                          const PipelineConfigInfo &configInfo);
    ~GhostGraphicsPipeline();

	vk::Pipeline operator*() const {return *m_graphicsPipeline;}
};

} // namespace Ghost
