#include <Ghost/ghostImage.hpp>

namespace Ghost {
GhostImage::GhostImage(VulkanDevice &device, uint32_t width, uint32_t height,
                       vk::Format format, vk::ImageTiling tiling,
                       vk::ImageUsageFlags usage,
                       vk::MemoryPropertyFlags properties,
                       vk::ImageAspectFlags aspectFlags)
    : m_device(device), m_format(format), m_aspectFlags(aspectFlags) {
	m_extent.setWidth(width).setHeight(height).setDepth(1);

    vk::ImageCreateInfo imageInfo{};
    imageInfo.setImageType(vk::ImageType::e2D)
        .setExtent(m_extent)
        .setMipLevels(m_mipLevels)
        .setArrayLayers(m_arrayLayers)
        .setFormat(m_format)
        .setTiling(tiling)
        .setInitialLayout(m_imageLayout)
        .setUsage(usage)
        .setSamples(vk::SampleCountFlagBits::e1)
        .setSharingMode(vk::SharingMode::eExclusive);

    m_image = vk::raii::Image(m_device.get(), imageInfo);

    vk::MemoryRequirements memRequirements = m_image.getMemoryRequirements();
    vk::MemoryAllocateInfo allocInfo{};
    allocInfo.setAllocationSize(memRequirements.size)
        .setMemoryTypeIndex(m_device.get().findMemoryType(
            memRequirements.memoryTypeBits, properties));

    m_memory = vk::raii::DeviceMemory(m_device.get(), allocInfo);

    m_image.bindMemory(m_memory, 0);

    vk::ImageViewCreateInfo viewInfo{};
    viewInfo.setImage(*m_image)
        .setViewType(vk::ImageViewType::e2D)
        .setFormat(m_format);

    viewInfo.subresourceRange.setAspectMask(m_aspectFlags)
        .setBaseMipLevel(0)
        .setLevelCount(m_mipLevels)
        .setBaseArrayLayer(0)
        .setLayerCount(m_arrayLayers);

    m_imageView = vk::raii::ImageView(m_device.get(), viewInfo);
}

void GhostImage::transitionImageLayout(vk::raii::CommandBuffer &cmd,
                                      vk::ImageLayout newLayout) {
    vk::ImageMemoryBarrier barrier{};
    barrier.oldLayout = m_imageLayout;
    barrier.newLayout = newLayout;

    barrier.srcQueueFamilyIndex = vk::QueueFamilyIgnored;
    barrier.dstQueueFamilyIndex = vk::QueueFamilyIgnored;

    barrier.setImage(m_image);

    barrier.subresourceRange.aspectMask = m_aspectFlags;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = m_mipLevels;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = m_arrayLayers;

    vk::PipelineStageFlags sourceStage;
    vk::PipelineStageFlags destinationStage;

    if (m_imageLayout == vk::ImageLayout::eUndefined &&
        newLayout == vk::ImageLayout::eTransferDstOptimal) {
        barrier.srcAccessMask = vk::AccessFlagBits::eNone;
        barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;

        sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
        destinationStage = vk::PipelineStageFlagBits::eTransfer;
    } else if (m_imageLayout == vk::ImageLayout::eTransferDstOptimal &&
               newLayout == vk::ImageLayout::eShaderReadOnlyOptimal) {
        barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
        barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

        sourceStage = vk::PipelineStageFlagBits::eTransfer;
        destinationStage = vk::PipelineStageFlagBits::eFragmentShader;
    } else {
        throw std::invalid_argument(
            "GhostImage: Unsupported layout transition requested!");
    }

    cmd.pipelineBarrier(sourceStage, destinationStage, vk::DependencyFlags(),
                        nullptr, nullptr, barrier);

    m_imageLayout = newLayout;
}

} // namespace Ghost
