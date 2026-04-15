#pragma once
#include <Ghost/vulkanInstance.hpp>
#include <Ghost/windowGLFW.hpp>
#include <vulkan/vulkan_raii.hpp>

namespace Ghost {
class GhostSurface {
    vk::raii::SurfaceKHR m_surface = nullptr;

  public:
    GhostSurface(const VulkanInstance &instance, const WindowGLFW &window);

    operator const vk::raii::SurfaceKHR &() const { return m_surface; }
	vk::SurfaceKHR operator*() const { return *m_surface; }
};
} // namespace Ghost
