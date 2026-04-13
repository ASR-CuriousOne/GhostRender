#pragma once
#include <GLFW/glfw3.h>
#include <Ghost/vulkanDevice.hpp>
#include <Ghost/vulkanInstance.hpp>
#include <Ghost/windowGLFW.hpp>
#include <vulkan/vulkan.hpp>

namespace Ghost {
class GhostRender {
  private:
    WindowGLFW m_window;
    VulkanInstance m_instance;
    VulkanDevice m_device;

  public:
    GhostRender();
    ~GhostRender();

    void run();
};
} // namespace Ghost
