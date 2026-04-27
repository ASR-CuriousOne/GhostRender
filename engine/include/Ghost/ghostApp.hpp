#pragma once
#include <Ghost/ghostCamera.hpp>
#include <Ghost/ghostGameObject.hpp>
#include <Ghost/ghostRenderer.hpp>
#include <Ghost/ghostSurface.hpp>
#include <Ghost/simpleRenderSystem.hpp>
#include <Ghost/vulkanDevice.hpp>
#include <Ghost/vulkanInstance.hpp>
#include <Ghost/windowGLFW.hpp>
#include <atomic>
#include <filesystem>

namespace Ghost {
struct Ubo {
    glm::mat4 projection{1.f};
    glm::mat4 view{1.f};
};

class GhostApp {
  public:
    static std::atomic<bool> s_quitFlag;
    GhostApp();
    ~GhostApp();

    void run();

    void loadGameObjects();

  private:
    WindowGLFW m_window;
    VulkanInstance m_instance;
    GhostSurface m_surface;
    VulkanDevice m_device;
    GhostRenderer m_renderer;

    vk::raii::DescriptorSetLayout m_descriptorSetLayout = nullptr;
    vk::raii::DescriptorPool m_descriptorPool = nullptr;
    std::vector<vk::raii::DescriptorSet> m_descriptorSets;

    std::vector<std::unique_ptr<GhostBuffer>> m_uniformBuffers;

    std::vector<GhostGameObject> m_gameObjects;

    void initDescriptors();

    void updateUniformBuffer(uint32_t currentImage, const GhostCamera &camera);
};
} // namespace Ghost
