#pragma once
#include <Ghost/Core/ghostImage.hpp>
#include <Ghost/Resources/textureBase.hpp>
#include <filesystem>
#include <vulkan/vulkan_raii.hpp>

namespace Ghost {
class GhostTexture : public TextureBase {
  public:
    GhostTexture(VulkanDevice &device, const std::filesystem::path &imagePath);
    ~GhostTexture() override = default;
};
} // namespace Ghost
