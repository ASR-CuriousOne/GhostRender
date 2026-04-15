#pragma once
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>

namespace Ghost {

struct QueueFamilyIndicies {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    void findQueueFamily(const vk::raii::PhysicalDevice &physicalDevice,
                         const vk::raii::SurfaceKHR &surface);
    bool isComplete();
};

class VulkanDevice {
    vk::raii::PhysicalDevices m_physicalDevices;
    vk::raii::PhysicalDevice m_physicalDevice = nullptr;
    QueueFamilyIndicies m_queueFamilyIndicies;

    vk::raii::Device m_device = nullptr;
    vk::raii::Queue m_graphicsQueue = nullptr;
	vk::raii::Queue m_presentQueue = nullptr;

    bool isDeviceSuitable(const vk::raii::PhysicalDevice &physicalDevice,
                          const vk::raii::SurfaceKHR &surface);

  public:
    VulkanDevice(const vk::raii::Instance &instance,
                 const vk::raii::SurfaceKHR &surface);
    std::string getDeviceName();
};

} // namespace Ghost
