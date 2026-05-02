#pragma once
#include <Ghost/ghostSurface.hpp>
#include <Ghost/vulkanDevice.hpp>
#include <Ghost/windowGLFW.hpp>
#include <cstdint>
#include <vulkan/vulkan_raii.hpp>

namespace Ghost {
class GhostSwapchain {
    vk::raii::SwapchainKHR m_swapchain = nullptr;
    std::vector<vk::Image> m_swapchainImages;
    std::vector<vk::raii::ImageView> m_swapchainImageViews;

    vk::Format m_swapchainImageFormat;
    vk::Extent2D m_swapchainExtent;

    vk::Format m_swapchainDepthFormat;
    std::vector<vk::raii::Image> m_depthImages;
    std::vector<vk::raii::DeviceMemory> m_depthImageMemorys;
    std::vector<vk::raii::ImageView> m_depthImageViews;

    vk::SurfaceFormatKHR chooseSwapSurfaceFormat(
        std::vector<vk::SurfaceFormatKHR> &availableFormats);

    vk::PresentModeKHR chooseSwapPresentMode(
        const std::vector<vk::PresentModeKHR> &availablePresentModes);

    vk::Extent2D
    chooseSwapExtent(const vk::SurfaceCapabilitiesKHR &capabilities,
                     const WindowGLFW &window);

  public:
    GhostSwapchain(const VulkanDevice &device, const WindowGLFW &window,
                   const GhostSurface &surface);
    ~GhostSwapchain();

    vk::SwapchainKHR operator*() const { return m_swapchain; }

    size_t getImageCount() const { return m_swapchainImages.size(); }
    const vk::Extent2D &getSwapchainExtent() { return m_swapchainExtent; }

    const vk::raii::ImageView &getDepthImageView(int index) const {
        return m_depthImageViews[index];
    }

    vk::Format getSwapchainImageFormat() const {
        return m_swapchainImageFormat;
    }

    const vk::raii::ImageView &getImageView(uint32_t index) const {
        return m_swapchainImageViews[index];
    }

    vk::ResultValue<uint32_t>
    aquireNextImage(const vk::raii::Semaphore &imageAvailableSemaphore);
};
} // namespace Ghost
