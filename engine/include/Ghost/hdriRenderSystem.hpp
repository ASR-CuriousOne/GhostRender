#pragma once
#include <Ghost/IRenderSystem.hpp>
#include <Ghost/ghostGraphicsPipeline.hpp>

namespace Ghost {
class HDRIRenderSystem : public IRenderSystem {
  public:
    HDRIRenderSystem(VulkanDevice &device, vk::RenderPass renderPass,
                     vk::DescriptorSetLayout globalSetLayout,
                     vk::DescriptorSetLayout hdriSetLayout);
    ~HDRIRenderSystem() override = default;

    void render(const FrameInfo &frameInfo,
                std::vector<GhostRenderObject> &renderObjects,
                const vk::raii::DescriptorSet &globalDescriptorSet) override;

    void setHdriDescriptorSet(vk::raii::DescriptorSet set) {
        m_hdriDescriptorSet = std::move(set);
    }

  private:
    void createPipelineLayout(vk::DescriptorSetLayout globalSetLayout,
                              vk::DescriptorSetLayout hdriSetLayout);
    void createPipeline(vk::RenderPass renderPass);
	void loadCubeModel();

    VulkanDevice &m_device;
    vk::raii::PipelineLayout m_pipelineLayout = nullptr;
    std::unique_ptr<GhostGraphicsPipeline> m_graphicsPipeline;
    vk::raii::DescriptorSet m_hdriDescriptorSet = nullptr;

	std::unique_ptr<GhostModel> m_cubeModel;
};
} // namespace Ghost
