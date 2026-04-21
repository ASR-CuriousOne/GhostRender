#pragma once
#include "vulkan/vulkan.hpp"
#include <Ghost/queueFamilyIndicies.hpp>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>

namespace Ghost {

class VulkanDevice {

    vk::raii::PhysicalDevices m_physicalDevices;
    vk::raii::PhysicalDevice m_physicalDevice = nullptr;

    const std::vector<const char *> m_deviceExtensions = {
        vk::KHRSwapchainExtensionName};

    QueueFamilyIndicies m_queueFamilyIndicies;

    vk::raii::Device m_device = nullptr;

    vk::raii::Queue m_graphicsQueue = nullptr;
    vk::raii::Queue m_presentQueue = nullptr;

    bool isDeviceSuitable(const vk::raii::PhysicalDevice &physicalDevice,
                          const vk::raii::SurfaceKHR &surface);

    bool checkExtensions(const vk::raii::PhysicalDevice &physicalDevice);

  public:
    VulkanDevice(const vk::raii::Instance &instance,
                 const vk::raii::SurfaceKHR &surface);

    operator const vk::raii::Device &() const { return m_device; }
    const vk::raii::Device *operator->() const { return &m_device; }
    operator const vk::raii::PhysicalDevice &() const {
        return m_physicalDevice;
    }

    const QueueFamilyIndicies &getQueueFamilyIndices() const {
        return m_queueFamilyIndicies;
    }

	uint32_t findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties) const;

    void submitGraphicsQueue(const vk::SubmitInfo &submitInfo,
                             const vk::Fence &fence);
    vk::Result submitPresentQueue(const vk::PresentInfoKHR &presentInfo);

    std::string getDeviceName();
};

} // namespace Ghost
