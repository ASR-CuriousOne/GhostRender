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

class GhostDescriptorPool {
  public:
    class Builder {
      public:
        Builder(VulkanDevice &device) : m_device{device} {}

        Builder &addPoolSize(vk::DescriptorType descriptorType, uint32_t count);
        Builder &setPoolFlags(vk::DescriptorPoolCreateFlags flags);
        Builder &setMaxSets(uint32_t count);
        std::unique_ptr<GhostDescriptorPool> build() const;

      private:
        VulkanDevice &m_device;
        std::vector<vk::DescriptorPoolSize> m_poolSizes{};
        uint32_t m_maxSets = 1000;
        vk::DescriptorPoolCreateFlags m_poolFlags =
            vk::DescriptorPoolCreateFlags();
    };

    GhostDescriptorPool(VulkanDevice &device, uint32_t maxSets,
                        vk::DescriptorPoolCreateFlags poolFlags,
                        const std::vector<vk::DescriptorPoolSize> &poolSizes);

    ~GhostDescriptorPool();
    GhostDescriptorPool(const GhostDescriptorPool &) = delete;
    GhostDescriptorPool &operator=(const GhostDescriptorPool &) = delete;

    std::vector<vk::raii::DescriptorSet>
    allocateDescriptorSets(uint32_t descriptorSetCount,
                           const vk::DescriptorSetLayout &descriptorSetLayout);

    vk::DescriptorPool getDescriptorPool() const { return *m_descriptorPool; }
    operator vk::DescriptorPool() const { return *m_descriptorPool; }

  private:
    VulkanDevice &m_device;
    vk::raii::DescriptorPool m_descriptorPool = nullptr;
};

} // namespace Ghost
