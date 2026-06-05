#include <Ghost/Core/ghostBuffer.hpp>
#include <Ghost/Resources/ghostTexture.hpp>
#include <format>
#include <iostream>
#include <span>
#define STB_IMAGE_IMPLEMENTATION
#include <vendor/stb_image.h>

namespace Ghost {
GhostTexture::GhostTexture(VulkanDevice &device,
                           const std::filesystem::path &imagePath)
    : TextureBase(device) {
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

    createTextureImage(pixels, imageSize, texWidth, texHeight,
                       vk::Format::eR8G8B8A8Srgb);

    stbi_image_free(pixels);
}

} // namespace Ghost
