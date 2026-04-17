#pragma once
#include <Ghost/ghostSurface.hpp>
#include <Ghost/vulkanDevice.hpp>
#include <Ghost/windowGLFW.hpp>
#include <vulkan/vulkan_raii.hpp>

namespace Ghost {
class GhostSwapchain {
    vk::raii::SwapchainKHR m_swapchain = nullptr;
    std::vector<vk::Image> m_swapchainImages;
    std::vector<vk::raii::ImageView> m_swapchainImageViews;
    vk::Format m_swapchainImageFormat;
    vk::Extent2D m_swapchainExtent;

    vk::raii::RenderPass m_renderPass = nullptr;

    vk::SurfaceFormatKHR chooseSwapSurfaceFormat(
        std::vector<vk::SurfaceFormatKHR> &availableFormats);

    vk::PresentModeKHR chooseSwapPresentMode(
        const std::vector<vk::PresentModeKHR> &availablePresentModes);

    vk::Extent2D
    chooseSwapExtent(const vk::SurfaceCapabilitiesKHR &capabilities,
                     const WindowGLFW &window);

    void createRenderPass(const VulkanDevice &device);

  public:
    GhostSwapchain(const VulkanDevice &device, const WindowGLFW &window,
                   const GhostSurface &surface);
    ~GhostSwapchain();

    const vk::Extent2D &getSwapchainExtent() { return m_swapchainExtent; }
    const vk::raii::RenderPass &getRenderPass() { return m_renderPass; }
};
} // namespace Ghost
