#pragma once
#include <GLFW/glfw3.h>
#include <Ghost/ghostSurface.hpp>
#include <Ghost/ghostSwapchain.hpp>
#include <Ghost/vulkanDevice.hpp>
#include <Ghost/vulkanInstance.hpp>
#include <Ghost/windowGLFW.hpp>
#include <atomic>
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

  public:
    static std::atomic<bool> s_quitFlag;
    GhostRender();
    ~GhostRender();

    void run();
};
} // namespace Ghost
