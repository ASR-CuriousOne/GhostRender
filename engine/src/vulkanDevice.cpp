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

    float queuePriority = 1.0f;
    vk::DeviceQueueCreateInfo queueCreateInfo(
        {}, m_queueFamilyIndicies.graphicsFamily.value(), 1, &queuePriority);

    vk::PhysicalDeviceFeatures deviceFeatures;

    vk::DeviceCreateInfo createInfo({}, 1, &queueCreateInfo, 0, nullptr, 0,
                                    nullptr, &deviceFeatures);

    m_device = vk::raii::Device(m_physicalDevice, createInfo);

    m_graphicsQueue = vk::raii::Queue(
        m_device, m_queueFamilyIndicies.graphicsFamily.value(), 0);
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
