#include <Ghost/ghostBuffer.hpp>
#include <Ghost/ghostTexture.hpp>
#include <format>
#include <iostream>
#include <span>
#define STB_IMAGE_IMPLEMENTATION
#include <vendor/stb_image.h>

namespace Ghost {
GhostTexture::GhostTexture(VulkanDevice &device,
                           const std::filesystem::path &imagePath)
    : m_device(device) {
    int texWidth, texHeight, texChannels;
    stbi_uc *pixels = stbi_load(imagePath.string().c_str(), &texWidth,
                                &texHeight, &texChannels, STBI_rgb_alpha);
    vk::DeviceSize imageSize = texWidth * texHeight * 4;

    std::clog << std::format("Width: {} Height: {} Channels: {}", texWidth,
                             texHeight, texChannels)
              << std::endl;

    if (!pixels) {
        throw std::runtime_error("failed to load texture image!");
    }

    GhostBuffer stagingBuffer(m_device, imageSize,
                              vk::BufferUsageFlagBits::eTransferSrc,
                              vk::MemoryPropertyFlagBits::eHostVisible |
                                  vk::MemoryPropertyFlagBits::eHostCoherent);

    stagingBuffer.writeToBuffer(
        std::span<const stbi_uc>(pixels, static_cast<uint32_t>(imageSize)));

    stbi_image_free(pixels);

    m_ghostImage = std::make_unique<GhostImage>(
        m_device, texWidth, texHeight, vk::Format::eR8G8B8A8Srgb,
        vk::ImageTiling::eOptimal,
        vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
        vk::MemoryPropertyFlagBits::eDeviceLocal,
        vk::ImageAspectFlagBits::eColor);

    auto commandBuffer = m_device.beginSingleTimeCommands();

    m_ghostImage->transitionImageLayout(commandBuffer,
                                        vk::ImageLayout::eTransferDstOptimal);
    copyBufferToImage(commandBuffer, stagingBuffer.getBuffer(),
                      *m_ghostImage->getImage(), texWidth, texHeight);
    m_ghostImage->transitionImageLayout(
        commandBuffer, vk::ImageLayout::eShaderReadOnlyOptimal);

    m_device.endSingleTimeCommands(commandBuffer);

    createTextureSampler();
}

void GhostTexture::createTextureSampler() {
    vk::SamplerCreateInfo createInfo;

    createInfo.setMagFilter(vk::Filter::eLinear)
        .setMinFilter(vk::Filter::eLinear)
        .setAddressModeU(vk::SamplerAddressMode::eRepeat)
        .setAddressModeV(vk::SamplerAddressMode::eRepeat)
        .setAddressModeW(vk::SamplerAddressMode::eRepeat)
        .setAnisotropyEnable(vk::True)
        .setMaxAnisotropy(
            m_device.getPhysicalDeviceProperties().limits.maxSamplerAnisotropy)
        .setBorderColor(vk::BorderColor::eIntOpaqueBlack)
        .setUnnormalizedCoordinates(vk::False)
        .setCompareEnable(vk::False)
        .setCompareOp(vk::CompareOp::eAlways)
        .setMipmapMode(vk::SamplerMipmapMode::eLinear)
        .setMipLodBias(0.0f)
        .setMinLod(0.0f)
        .setMaxLod(0.0f);

    m_sampler = vk::raii::Sampler(m_device, createInfo);
}

void GhostTexture::copyBufferToImage(vk::raii::CommandBuffer &commandBuffer,
                                     const vk::Buffer &buffer,
                                     const vk::Image &image, uint32_t width,
                                     uint32_t height) {
    vk::BufferImageCopy region;

    region.setBufferOffset(0)
        .setBufferImageHeight(0)
        .setBufferRowLength(0)
        .imageSubresource.setAspectMask(vk::ImageAspectFlagBits::eColor)
        .setMipLevel(0)
        .setBaseArrayLayer(0)
        .setLayerCount(1);

    region.imageExtent.setWidth(width).setHeight(height).setDepth(1);

    commandBuffer.copyBufferToImage(
        buffer, image, vk::ImageLayout::eTransferDstOptimal, region);
}

} // namespace Ghost
