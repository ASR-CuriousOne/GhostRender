#pragma once
#include <Ghost/vulkanDevice.hpp>
#include <vulkan/vulkan_raii.hpp>

namespace Ghost {
class GhostCommandPool {
    vk::raii::CommandPool m_commandPool = nullptr;

  public:
    GhostCommandPool(const VulkanDevice &device);
    ~GhostCommandPool();
};
} // namespace Ghost
