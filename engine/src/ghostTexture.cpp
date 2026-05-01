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
    createTextureImage(imagePath);
    createTextureImageView();
    createTextureSampler();
}

void GhostTexture::createTextureImage(const std::filesystem::path &imagePath) {
    int texWidth, texHeight, texChannels;
    stbi_uc *pixels = stbi_load(imagePath.string().c_str(), &texWidth, &texHeight,
                                &texChannels, STBI_rgb_alpha);
    vk::DeviceSize imageSize = texWidth * texHeight * 4;

	std::clog << std::format("Width: {} Height: {} Channels: {}", texWidth, texHeight, texChannels) << std::endl;

    if (!pixels) {
        throw std::runtime_error("failed to load texture image!");
    }

    GhostBuffer stagingBuffer(m_device, imageSize,
                              vk::BufferUsageFlagBits::eTransferSrc,
                              vk::MemoryPropertyFlagBits::eHostVisible |
                                  vk::MemoryPropertyFlagBits::eHostCoherent);

    stagingBuffer.writeToBuffer(std::span<const stbi_uc>(
        pixels, static_cast<uint32_t>(imageSize)));

    stbi_image_free(pixels);

    vk::ImageCreateInfo imageCreateInfo;

    imageCreateInfo.setImageType(vk::ImageType::e2D)
        .extent.setWidth(texWidth)
        .setHeight(texHeight)
        .setDepth(1);

    imageCreateInfo.setMipLevels(1)
        .setArrayLayers(1)
        .setFormat(vk::Format::eR8G8B8A8Srgb)
        .setTiling(vk::ImageTiling::eOptimal)
        .setInitialLayout(vk::ImageLayout::eUndefined)
        .setUsage(vk::ImageUsageFlagBits::eTransferDst |
                  vk::ImageUsageFlagBits::eSampled)
        .setSharingMode(vk::SharingMode::eExclusive)
        .setSamples(vk::SampleCountFlagBits::e1);

    m_texture = vk::raii::Image(m_device, imageCreateInfo);

    vk::MemoryRequirements memRequirements = m_texture.getMemoryRequirements();

    vk::MemoryAllocateInfo allocInfo{};
    allocInfo.setAllocationSize(memRequirements.size);

    allocInfo.memoryTypeIndex =
        m_device.findMemoryType(memRequirements.memoryTypeBits,
                                vk::MemoryPropertyFlagBits::eDeviceLocal);

    m_imageMemory = vk::raii::DeviceMemory(m_device, allocInfo);

    m_texture.bindMemory(m_imageMemory, 0);

    transitionImageLayout(m_texture, vk::Format::eR8G8B8A8Srgb,
                          vk::ImageLayout::eUndefined,
                          vk::ImageLayout::eTransferDstOptimal);

    copyBufferToImage(stagingBuffer.getBuffer(), m_texture, texWidth,
                      texHeight);

    transitionImageLayout(m_texture, vk::Format::eR8G8B8A8Srgb,
                          vk::ImageLayout::eTransferDstOptimal,
                          vk::ImageLayout::eShaderReadOnlyOptimal);
}

void GhostTexture::createTextureImageView() {
    vk::ImageViewCreateInfo createInfo(
        {}, m_texture, vk::ImageViewType::e2D, vk::Format::eR8G8B8A8Srgb, {},
        vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1));

    m_imageView = vk::raii::ImageView(m_device, createInfo);
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

void GhostTexture::transitionImageLayout(vk::raii::Image &image,
                                         vk::Format format,
                                         vk::ImageLayout oldLayout,
                                         vk::ImageLayout newLayout) {
    auto commandBuffer = m_device.beginSingleTimeCommands();

    vk::ImageMemoryBarrier barrier;

    barrier.setOldLayout(oldLayout)
        .setNewLayout(newLayout)
        .setImage(image)
        .subresourceRange.setAspectMask(vk::ImageAspectFlagBits::eColor)
        .setBaseMipLevel(0)
        .setLevelCount(1)
        .setBaseArrayLayer(0)
        .setLayerCount(1);

    vk::PipelineStageFlags sourceStage;
    vk::PipelineStageFlags destinationStage;

    if (oldLayout == vk::ImageLayout::eUndefined &&
        newLayout == vk::ImageLayout::eTransferDstOptimal) {
        barrier.setDstAccessMask(vk::AccessFlagBits::eTransferWrite);

        sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
        destinationStage = vk::PipelineStageFlagBits::eTransfer;
    } else if (oldLayout == vk::ImageLayout::eTransferDstOptimal &&
               newLayout == vk::ImageLayout::eShaderReadOnlyOptimal) {
        barrier.setSrcAccessMask(vk::AccessFlagBits::eTransferWrite);
        barrier.setDstAccessMask(vk::AccessFlagBits::eShaderRead);

        sourceStage = vk::PipelineStageFlagBits::eTransfer;
        destinationStage = vk::PipelineStageFlagBits::eFragmentShader;
    } else {
        throw std::invalid_argument("unsupported layout transition!");
    }

    commandBuffer.pipelineBarrier(sourceStage, destinationStage, {}, nullptr,
                                  nullptr, barrier);

    m_device.endSingleTimeCommands(commandBuffer);
}

void GhostTexture::copyBufferToImage(const vk::Buffer &buffer,
                                     const vk::Image &image, uint32_t width,
                                     uint32_t height) {
    auto commandBuffer = m_device.beginSingleTimeCommands();

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

    m_device.endSingleTimeCommands(commandBuffer);
}

} // namespace Ghost
