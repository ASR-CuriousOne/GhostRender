#pragma once
#include <Ghost/ghostImage.hpp>
#include <Ghost/textureBase.hpp>
#include <Ghost/vulkanDevice.hpp>
#include <filesystem>
#include <vulkan/vulkan_raii.hpp>

namespace Ghost {
class GhostTexture : public TextureBase {
  public:
    GhostTexture(VulkanDevice &device, const std::filesystem::path &imagePath);
    ~GhostTexture() override = default;
};
} // namespace Ghost
