#pragma once
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>

namespace Ghost {

struct QueueFamilyIndicies {
    std::optional<uint32_t> graphicsFamily;

    void findQueueFamily(const vk::raii::PhysicalDevice &physicalDevice);
    bool isComplete();
};

class VulkanDevice {
    vk::raii::PhysicalDevices m_physicalDevices;
    vk::raii::PhysicalDevice m_physicalDevice = nullptr;
    QueueFamilyIndicies m_queueFamilyIndicies;

    vk::raii::Device m_device = nullptr;

    bool isDeviceSuitable(const vk::raii::PhysicalDevice &physicalDevice);

  public:
    VulkanDevice(const vk::raii::Instance &instance);
    std::string getDeviceName();
};

} // namespace Ghost
