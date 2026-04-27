#include <Ghost/ghostDescriptors.hpp>
#include <stdexcept>
#include <vector>

namespace Ghost {
GhostDescriptorSetLayout::Builder &
GhostDescriptorSetLayout::Builder::addBinding(uint32_t binding,
                                              vk::DescriptorType descriptorType,
                                              vk::ShaderStageFlags stageFlags,
                                              uint32_t count) {

    if (m_bindings.count(binding) != 0) {
        throw std::runtime_error("Binding already in use");
    }

    vk::DescriptorSetLayoutBinding layoutBinding{};
    layoutBinding.setBinding(binding)
        .setDescriptorType(descriptorType)
        .setDescriptorCount(count)
        .setStageFlags(stageFlags);

    m_bindings[binding] = layoutBinding;

    return *this;
}

std::unique_ptr<GhostDescriptorSetLayout>
GhostDescriptorSetLayout::Builder::build() const {
    return std::make_unique<GhostDescriptorSetLayout>(m_device, m_bindings);
}

GhostDescriptorSetLayout::GhostDescriptorSetLayout(
    VulkanDevice &device,
    std::unordered_map<uint32_t, vk::DescriptorSetLayoutBinding> bindings)
    : m_device{device}, m_bindings{bindings} {

    std::vector<vk::DescriptorSetLayoutBinding> setLayoutBindings{};
    for (auto &[key, value] : bindings) {
        setLayoutBindings.push_back(value);
    }

    vk::DescriptorSetLayoutCreateInfo descriptorSetLayoutInfo{};
    descriptorSetLayoutInfo
        .setBindingCount(static_cast<uint32_t>(setLayoutBindings.size()))
        .setPBindings(setLayoutBindings.data());

    m_descriptorSetLayout =
        vk::raii::DescriptorSetLayout(m_device, descriptorSetLayoutInfo);
}

GhostDescriptorSetLayout::~GhostDescriptorSetLayout() {}

} // namespace Ghost
