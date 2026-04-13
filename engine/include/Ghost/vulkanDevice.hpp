#pragma once
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>

namespace Ghost {

class VulkanDevice {
    vk::raii::PhysicalDevices m_physicalDevices;
    vk::raii::PhysicalDevice m_physicalDevice = nullptr;

    bool isDeviceSuitable(const vk::raii::PhysicalDevice &physicalDevice);

  public:
    VulkanDevice(const vk::raii::Instance &instance);
	std::string getDeviceName();
};

} // namespace Ghost
