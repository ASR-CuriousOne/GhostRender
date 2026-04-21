#pragma once
#include <Ghost/ghostCommandPool.hpp>
#include <Ghost/ghostSurface.hpp>
#include <Ghost/ghostSwapchain.hpp>
#include <Ghost/vulkanDevice.hpp>
#include <Ghost/windowGLFW.hpp>
#include <cstdint>
#include <memory>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>

namespace Ghost {
const int MAX_FRAMES_IN_FLIGHT = 2;
class GhostRenderer {
  public:
    GhostRenderer(WindowGLFW &window, VulkanDevice &device,
                  GhostSurface &surface);
    ~GhostRenderer();

    vk::raii::CommandBuffer &beginFrame();
    void endFrame();
    void beginSwapChainRenderPass(const vk::raii::CommandBuffer &commandBuffer);
    void endSwapChainRenderPass(const vk::raii::CommandBuffer &commandBuffer);

    int getFrameIndex() const { return m_currentFrame; }
    vk::RenderPass getSwapChainRenderPass() const {
        return m_swapchain->getRenderPass();
    }

    bool isFrameInProgress() { return m_isFrameStarted; }

  private:
    void createCommandBuffers();
    void createSyncObjects();
    void recreateSwapchain();

    WindowGLFW &m_window;
    VulkanDevice &m_device;
    GhostSurface &m_surface;

    std::unique_ptr<GhostSwapchain> m_swapchain;
    GhostCommandPool m_commandPool;
    std::vector<vk::raii::CommandBuffer> m_commandBuffers;

    std::vector<vk::raii::Semaphore> m_imageAvailableSemaphores;
    std::vector<vk::raii::Semaphore> m_renderFinishedSemaphores;
    std::vector<vk::raii::Fence> m_inFlightFences;

    uint32_t m_currentFrame = 0;
    uint32_t m_currentImageIndex = 0;
    bool m_isFrameStarted = false;
};
} // namespace Ghost
