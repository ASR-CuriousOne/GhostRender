#pragma once
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

    const vk::raii::ImageView &getImageView() const { return m_imageView; }
    const vk::raii::Sampler &getSampler() const { return m_sampler; }
    vk::ImageLayout getImageLayout() const {
        return vk::ImageLayout::eShaderReadOnlyOptimal;
    }

    vk::DescriptorImageInfo descriptorInfo() {
        return vk::DescriptorImageInfo(m_sampler, m_imageView,
                                       vk::ImageLayout::eShaderReadOnlyOptimal);
    }

  private:
    VulkanDevice &m_device;

    vk::raii::DeviceMemory m_imageMemory = nullptr;
    vk::raii::Image m_texture = nullptr;
    vk::raii::ImageView m_imageView = nullptr;
    vk::raii::Sampler m_sampler = nullptr;

    void createTextureImage(const std::filesystem::path &imagePath);
    void createTextureImageView();
    void createTextureSampler();

    void transitionImageLayout(vk::raii::Image &image, vk::Format format,
                               vk::ImageLayout oldLayout,
                               vk::ImageLayout newLayout);
    void copyBufferToImage(const vk::Buffer &buffer, const vk::Image &image,
                           uint32_t width, uint32_t height);
};
} // namespace Ghost
