#pragma once
#include <Ghost/ghostGraphicsPipeline.hpp>
#include <Ghost/ghostRenderer.hpp>
#include <Ghost/ghostSurface.hpp>
#include <Ghost/vulkanDevice.hpp>
#include <Ghost/vulkanInstance.hpp>
#include <Ghost/windowGLFW.hpp>
#include <Ghost/ghostModel.hpp>
#include <atomic>
#include <filesystem>

namespace Ghost {
class GhostApp {
  public:
    static std::atomic<bool> s_quitFlag;
    GhostApp();
    ~GhostApp();

    void run();

  private:
    WindowGLFW m_window;
    VulkanInstance m_instance;
    GhostSurface m_surface;
    VulkanDevice m_device;
    GhostRenderer m_renderer;

    vk::raii::PipelineLayout m_pipelineLayout = nullptr;
    std::unique_ptr<GhostGraphicsPipeline> m_graphicsPipeline;

    std::filesystem::path m_vertShaderPath;
    std::filesystem::path m_fragShaderPath;

    std::unique_ptr<GhostModel> m_model;
};
} // namespace Ghost
