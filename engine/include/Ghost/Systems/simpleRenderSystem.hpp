#pragma once
#include <Ghost/Systems/IRenderSystem.hpp>
#include <Ghost/Core/ghostGraphicsPipeline.hpp>
#include <Ghost/Utils/ghostRenderObject.hpp>
#include <Ghost/Core/vulkanDevice.hpp>

namespace Ghost {
class SimpleRenderSystem : public IRenderSystem {
  public:
    SimpleRenderSystem(VulkanDevice &device, vk::RenderPass renderPass,
                       vk::DescriptorSetLayout globalSetLayout,
                       vk::DescriptorSetLayout textureSetLayout);
    ~SimpleRenderSystem();

    void render(const FrameInfo &frameInfo,
                std::vector<GhostRenderObject> &renderObjects,
                const vk::raii::DescriptorSet &globalDescriptorSet) override;

  private:
    void createPipelineLayout(vk::DescriptorSetLayout globalSetLayout,
                              vk::DescriptorSetLayout textureSetLayout);
    void createPipeline(vk::RenderPass renderPass);

    VulkanDevice &m_device;
    vk::raii::PipelineLayout m_pipelineLayout = nullptr;
    std::unique_ptr<GhostGraphicsPipeline> m_graphicsPipeline;
};
} // namespace Ghost
