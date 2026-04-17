#pragma once
#include <GLFW/glfw3.h>
#include <Ghost/ghostGraphicsPipeline.hpp>
#include <Ghost/ghostSurface.hpp>
#include <Ghost/ghostSwapchain.hpp>
#include <Ghost/vulkanDevice.hpp>
#include <Ghost/vulkanInstance.hpp>
#include <Ghost/windowGLFW.hpp>
#include <atomic>
#include <filesystem>
#include <memory>
#include <vulkan/vulkan.hpp>

namespace Ghost {
class GhostRender {
  private:
    WindowGLFW m_window;
    VulkanInstance m_instance;
    GhostSurface m_surface;
    VulkanDevice m_device;

    std::unique_ptr<GhostSwapchain> m_swapchain;

    vk::raii::PipelineLayout m_pipelineLayout = nullptr;

	const std::filesystem::path c_vertShaderPath = "./shaders/vert.spv";
	const std::filesystem::path c_fragShaderPath = "./shaders/frag.spv";
    std::unique_ptr<GhostGraphicsPipeline> m_graphicsPipeline;

  public:
    static std::atomic<bool> s_quitFlag;
    GhostRender();
    ~GhostRender();

    void run();
};
} // namespace Ghost
