#pragma once
#include <GLFW/glfw3.h>
#include <Ghost/ghostCommandPool.hpp>
#include <Ghost/ghostGraphicsPipeline.hpp>
#include <Ghost/ghostSurface.hpp>
#include <Ghost/ghostSwapchain.hpp>
#include <Ghost/vulkanDevice.hpp>
#include <Ghost/vulkanInstance.hpp>
#include <Ghost/windowGLFW.hpp>
#include <atomic>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>

namespace Ghost {
const int MAX_FRAMES_IN_FLIGHT = 2;
class GhostRender {
  private:
    WindowGLFW m_window;
    VulkanInstance m_instance;
    GhostSurface m_surface;
    VulkanDevice m_device;
    GhostCommandPool m_commandPool;
    std::vector<vk::raii::CommandBuffer> m_commandBuffers;

    std::vector<vk::raii::Semaphore> m_imageAvailableSemaphores;
    std::vector<vk::raii::Semaphore> m_renderFinishedSemaphores;
    std::vector<vk::raii::Fence> m_inFlightFences;
	uint32_t m_currentFrame = 0;

    std::unique_ptr<GhostSwapchain> m_swapchain;

    vk::raii::PipelineLayout m_pipelineLayout = nullptr;

    const std::filesystem::path c_vertShaderPath = "./shaders/vert.spv";
    const std::filesystem::path c_fragShaderPath = "./shaders/frag.spv";
    std::unique_ptr<GhostGraphicsPipeline> m_graphicsPipeline;

    void createCommandBuffer();

    void recordCommandBuffer(const vk::raii::CommandBuffer &commandBuffer,
                             uint32_t imageIndex);

    void createSyncObjects();

    void drawFrame();

	void recreateSwapchain();

  public:
    static std::atomic<bool> s_quitFlag;
    GhostRender();
    ~GhostRender();

    void run();
};
} // namespace Ghost
