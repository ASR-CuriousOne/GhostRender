#pragma once

#include <Ghost/vulkanDevice.hpp>
#include <memory>
#include <unordered_map>
#include <vulkan/vulkan_raii.hpp>

namespace Ghost {
class GhostDescriptorSetLayout {
  public:
    class Builder {
      public:
        Builder(VulkanDevice &device) : m_device{device} {}

        Builder &addBinding(uint32_t binding, vk::DescriptorType descriptorType,
                            vk::ShaderStageFlags stageFlags,
                            uint32_t count = 1);

        std::unique_ptr<GhostDescriptorSetLayout> build() const;

      private:
        VulkanDevice &m_device;
        std::unordered_map<uint32_t, vk::DescriptorSetLayoutBinding>
            m_bindings{};
    };
    GhostDescriptorSetLayout(
        VulkanDevice &device,
        std::unordered_map<uint32_t, vk::DescriptorSetLayoutBinding> bindings);
    ~GhostDescriptorSetLayout();

    GhostDescriptorSetLayout(const GhostDescriptorSetLayout &) = delete;
    GhostDescriptorSetLayout &
    operator=(const GhostDescriptorSetLayout &) = delete;

    vk::DescriptorSetLayout getDescriptorSetLayout() const {
        return *m_descriptorSetLayout;
    }
    operator vk::DescriptorSetLayout() const { return *m_descriptorSetLayout; }

  private:
    VulkanDevice &m_device;
    vk::raii::DescriptorSetLayout m_descriptorSetLayout = nullptr;
    std::unordered_map<uint32_t, vk::DescriptorSetLayoutBinding> m_bindings;
};
} // namespace Ghost
