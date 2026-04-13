#include <Ghost/vulkanDevice.hpp>
#include <iostream>
#include <vulkan/vulkan_to_string.hpp>

namespace Ghost {
void QueueFamilyIndicies::findQueueFamily(
    const vk::raii::PhysicalDevice &physicalDevice) {
    auto queueFamilies = physicalDevice.getQueueFamilyProperties();
    uint32_t i = 0;
    for (const auto &queueFamily : queueFamilies) {
        if (queueFamily.queueFlags & vk::QueueFlags::BitsType::eGraphics) {
            graphicsFamily = i;
        }
        if (isComplete()) {
            break;
        }

        i++;
    }
}

bool QueueFamilyIndicies::isComplete() { return graphicsFamily.has_value(); }
} // namespace Ghost
