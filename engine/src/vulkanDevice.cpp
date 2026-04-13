#include <Ghost/vulkanDevice.hpp>
#include <stdexcept>

namespace Ghost {
VulkanDevice::VulkanDevice(const vk::raii::Instance &instance)
    : m_physicalDevices(instance) {
    for (size_t i = 0; i < m_physicalDevices.size(); i++) {
        const auto &device = m_physicalDevices[i];
        if (isDeviceSuitable(device)) {
            m_physicalDevice = std::move(device);
			m_queueFamilyIndicies.findQueueFamily(device);
            break;
        }
    }

    if (!(*m_physicalDevice)) {
        throw std::runtime_error("failed to find a suitable GPU!");
    }
}

bool VulkanDevice::isDeviceSuitable(
    const vk::raii::PhysicalDevice &physicalDevice) {
    auto features = physicalDevice.getFeatures();
    auto properties = physicalDevice.getProperties();

    QueueFamilyIndicies queueFamilyIndicies;
    queueFamilyIndicies.findQueueFamily(physicalDevice);

    return properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu &&
           queueFamilyIndicies.isComplete();
}

std::string VulkanDevice::getDeviceName() {
    return m_physicalDevice.getProperties().deviceName;
}
} // namespace Ghost
