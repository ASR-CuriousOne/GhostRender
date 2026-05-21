#pragma once
#include <Ghost/ghostRenderer.hpp>
#include <Ghost/vulkanDevice.hpp>
#include <Ghost/vulkanInstance.hpp>
#include <functional>
#include <memory>
#include <vector>
#include <vulkan/vulkan_raii.hpp>

namespace Ghost {

struct GhostRenderConfig {
    std::vector<const char *> instanceExtensions;
    std::function<vk::SurfaceKHR(VkInstance)> surfaceCreateCallback;
    uint32_t windowWidth;
    uint32_t windowHeight;
};

class GhostRender {
  public:
    GhostRender(GhostRenderConfig &config);
    ~GhostRender();

    GhostRender(const GhostRender &) = delete;
    GhostRender &operator=(const GhostRender &) = delete;

    vk::raii::CommandBuffer &beginFrame();
    void endFrame();
    void beginSwapChainRenderPass(const vk::raii::CommandBuffer &commandBuffer);
    void endSwapChainRenderPass(const vk::raii::CommandBuffer &commandBuffer);

    void resize(uint32_t width, uint32_t height);

    VulkanDevice &getDevice() { return *m_device; }
    vk::RenderPass getRenderPass() const {
        return m_renderer->getSwapChainRenderPass();
    }
    float getAspectRatio() const;

    int getFrameIndex() const { return m_renderer->getFrameIndex(); }
    bool isFrameInProgress() const { return m_renderer->isFrameInProgress(); }

	const VulkanDevice& getDevice() const {return *m_device;}

  private:
    std::unique_ptr<VulkanInstance> m_instance;
    std::unique_ptr<vk::raii::SurfaceKHR> m_surface;
    std::unique_ptr<VulkanDevice> m_device;
    std::unique_ptr<GhostRenderer> m_renderer;

    uint32_t m_width;
    uint32_t m_height;
    bool m_isMinimized = false;
};
} // namespace Ghost
