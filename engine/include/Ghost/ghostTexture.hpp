#pragma once
#include <Ghost/ghostImage.hpp>
#include <Ghost/vulkanDevice.hpp>
#include <filesystem>
#include <vulkan/vulkan_raii.hpp>

namespace Ghost {
class GhostTexture {
  public:
    GhostTexture(VulkanDevice &device, const std::filesystem::path &imagePath);
    ~GhostTexture() = default;

    GhostTexture(const GhostTexture &) = delete;
    GhostTexture &operator=(const GhostTexture &) = delete;

    const vk::raii::ImageView &getImageView() const {
        return m_ghostImage->getImageView();
    }
    const vk::raii::Sampler &getSampler() const { return m_sampler; }
    vk::ImageLayout getImageLayout() const {
        return vk::ImageLayout::eShaderReadOnlyOptimal;
    }

    vk::DescriptorImageInfo descriptorInfo() {
        return vk::DescriptorImageInfo(m_sampler, m_ghostImage->getImageView(),
                                       vk::ImageLayout::eShaderReadOnlyOptimal);
    }

  private:
    VulkanDevice &m_device;

    std::unique_ptr<GhostImage> m_ghostImage;
    vk::raii::Sampler m_sampler = nullptr;

    void createTextureSampler();
    void copyBufferToImage(vk::raii::CommandBuffer &commandBuffer,
                           const vk::Buffer &buffer, const vk::Image &image,
                           uint32_t width, uint32_t height);
};
} // namespace Ghost
