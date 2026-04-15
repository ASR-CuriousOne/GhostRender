#include <Ghost/vulkanDevice.hpp>
#include <set>
#include <stdexcept>

namespace Ghost {
VulkanDevice::VulkanDevice(const vk::raii::Instance &instance,
                           const vk::raii::SurfaceKHR &surface)
    : m_physicalDevices(instance) {
    for (size_t i = 0; i < m_physicalDevices.size(); i++) {
        const auto &device = m_physicalDevices[i];
        if (isDeviceSuitable(device, surface)) {
            m_physicalDevice = std::move(device);
            m_queueFamilyIndicies.findQueueFamily(device, surface);
            break;
        }
    }

    if (!(*m_physicalDevice)) {
        throw std::runtime_error("failed to find a suitable GPU!");
    }

    std::set<uint32_t> uniqueQueueFamilyIndicies = {
        m_queueFamilyIndicies.graphicsFamily.value(),
        m_queueFamilyIndicies.presentFamily.value()};

    float queuePriority = 1.0f;
    std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;

    for (auto index : uniqueQueueFamilyIndicies) {
        queueCreateInfos.push_back({{}, index, 1, &queuePriority});
    }

    vk::PhysicalDeviceFeatures deviceFeatures;

    vk::DeviceCreateInfo createInfo({}, queueCreateInfos.size(),
                                    queueCreateInfos.data(), 0, nullptr, 0,
                                    nullptr, &deviceFeatures);

    m_device = vk::raii::Device(m_physicalDevice, createInfo);

    m_graphicsQueue = vk::raii::Queue(
        m_device, m_queueFamilyIndicies.graphicsFamily.value(), 0);
    m_presentQueue = vk::raii::Queue(
        m_device, m_queueFamilyIndicies.presentFamily.value(), 0);
}

bool VulkanDevice::isDeviceSuitable(
    const vk::raii::PhysicalDevice &physicalDevice,
    const vk::raii::SurfaceKHR &surface) {
    auto features = physicalDevice.getFeatures();
    auto properties = physicalDevice.getProperties();

    QueueFamilyIndicies queueFamilyIndicies;
    queueFamilyIndicies.findQueueFamily(physicalDevice, surface);

    return properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu &&
           queueFamilyIndicies.isComplete();
}

std::string VulkanDevice::getDeviceName() {
    return m_physicalDevice.getProperties().deviceName;
}
} // namespace Ghost
