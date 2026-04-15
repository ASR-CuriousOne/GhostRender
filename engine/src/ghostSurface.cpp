#include <Ghost/ghostSurface.hpp>
#include <stdexcept>

namespace Ghost {
GhostSurface::GhostSurface(const VulkanInstance &instance,
                           const WindowGLFW &window) {
    VkSurfaceKHR rawSurface;
    if (glfwCreateWindowSurface(*instance, window, nullptr, &rawSurface) !=
        VK_SUCCESS) {
        throw std::runtime_error("Failed to create window surface!");
    }
    m_surface = vk::raii::SurfaceKHR(instance, rawSurface);
}
} // namespace Ghost
