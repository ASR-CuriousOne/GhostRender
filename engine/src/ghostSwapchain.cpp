#include <Ghost/ghostSwapchain.hpp>
#include <Ghost/swapChainDetails.hpp>
#include <Ghost/vulkanDevice.hpp>
#include <vulkan/vulkan.hpp>

namespace Ghost {
GhostSwapchain::GhostSwapchain(const VulkanDevice &device,
                               const WindowGLFW &window,
                               const GhostSurface &surface) {
    SwapChainSupportDetails swapChainSupport;
    swapChainSupport.querySwapChainSupport(device, surface);

    auto surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
    auto presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
    auto extent = chooseSwapExtent(swapChainSupport.capabilities, window);

    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

    if (swapChainSupport.capabilities.maxImageCount > 0 &&
        imageCount > swapChainSupport.capabilities.maxImageCount) {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    QueueFamilyIndicies indices = device.getQueueFamilyIndices();
    std::vector<uint32_t> queueFamilyIndices = {indices.graphicsFamily.value(),
                                                indices.presentFamily.value()};
    bool isConcurrent = (indices.graphicsFamily != indices.presentFamily);

    auto createInfo =
        vk::SwapchainCreateInfoKHR()
            .setSurface(*surface)
            .setMinImageCount(imageCount)
            .setImageFormat(surfaceFormat.format)
            .setImageColorSpace(surfaceFormat.colorSpace)
            .setImageExtent(extent)
            .setImageArrayLayers(1)
            .setImageUsage(vk::ImageUsageFlagBits::eColorAttachment)
            .setImageSharingMode(isConcurrent ? vk::SharingMode::eConcurrent
                                              : vk::SharingMode::eExclusive)
            .setPreTransform(swapChainSupport.capabilities.currentTransform)
            .setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque)
            .setPresentMode(presentMode)
            .setClipped(vk::True)
            .setOldSwapchain(nullptr);

    if (isConcurrent)
        createInfo.setQueueFamilyIndices(queueFamilyIndices);

    m_swapchain = vk::raii::SwapchainKHR(device, createInfo);

    m_swapchainImages = m_swapchain.getImages();
    m_swapchainImageFormat = surfaceFormat.format;
    m_swapchainExtent = extent;
}

GhostSwapchain::~GhostSwapchain() {}

vk::SurfaceFormatKHR GhostSwapchain::chooseSwapSurfaceFormat(
    std::vector<vk::SurfaceFormatKHR> &availableFormats) {
    for (const auto &availableFormat : availableFormats) {
        if (availableFormat.format == vk::Format::eB8G8R8A8Srgb &&
            availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
            return availableFormat;
        }
    }

    return availableFormats[0];
}

vk::PresentModeKHR GhostSwapchain::chooseSwapPresentMode(
    const std::vector<vk::PresentModeKHR> &availablePresentModes) {
    for (const auto &availablePresentMode : availablePresentModes) {
        if (availablePresentMode == vk::PresentModeKHR::eMailbox) {
            return availablePresentMode;
        }
    }

    return vk::PresentModeKHR::eFifo;
}

vk::Extent2D
GhostSwapchain::chooseSwapExtent(const vk::SurfaceCapabilitiesKHR &capabilities,
                                 const WindowGLFW &window) {
    if (capabilities.currentExtent.width !=
        std::numeric_limits<uint32_t>::max()) {
        return capabilities.currentExtent;
    } else {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);

        VkExtent2D actualExtent = {static_cast<uint32_t>(width),
                                   static_cast<uint32_t>(height)};

        actualExtent.width =
            std::clamp(actualExtent.width, capabilities.minImageExtent.width,
                       capabilities.maxImageExtent.width);
        actualExtent.height =
            std::clamp(actualExtent.height, capabilities.minImageExtent.height,
                       capabilities.maxImageExtent.height);

        return actualExtent;
    }
}

} // namespace Ghost
