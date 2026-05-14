#pragma once
#include <Ghost/frameInfo.hpp>
#include <Ghost/ghostGraphicsPipeline.hpp>
#include <Ghost/ghostRenderObject.hpp>
#include <Ghost/vulkanDevice.hpp>

namespace Ghost {
class SimpleRenderSystem {
  public:
    SimpleRenderSystem(VulkanDevice &device, vk::RenderPass renderPass,
                       vk::DescriptorSetLayout globalSetLayout,
                       vk::DescriptorSetLayout textureSetLayout);
    ~SimpleRenderSystem();

    void renderGameObjects(const FrameInfo &frameInfo,
                           std::vector<GhostRenderObject> &gameObjects,
                           const vk::raii::DescriptorSet &globalDescriptorSet);

  private:
    void createPipelineLayout(vk::DescriptorSetLayout globalSetLayout,
                              vk::DescriptorSetLayout textureSetLayout);
    void createPipeline(vk::RenderPass renderPass);

    VulkanDevice &m_device;
    vk::raii::PipelineLayout m_pipelineLayout = nullptr;
    std::unique_ptr<GhostGraphicsPipeline> m_graphicsPipeline;
};
} // namespace Ghost
