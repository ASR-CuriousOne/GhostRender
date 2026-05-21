#pragma once
#include <Ghost/frameInfo.hpp>
#include <Ghost/ghostCamera.hpp>
#include <Ghost/ghostDescriptorManager.hpp>
#include <Ghost/ghostRenderObject.hpp>
#include <Ghost/ghostRenderer.hpp>
#include <Ghost/vulkanDevice.hpp>

class Application {
  public:
    Application();
    virtual ~Application() = default;

    void run();
    void close();

  protected:
    virtual void onInit() = 0;
    virtual void onUpdate(float dt) = 0;
    virtual void onRender(const Ghost::FrameInfo &frameInfo) = 0;
    virtual void onShutdown() = 0;

    Ghost::WindowGLFW m_window;
    Ghost::VulkanInstance m_instance;
    Ghost::GhostSurface m_surface;
    Ghost::VulkanDevice m_device;
    Ghost::GhostRenderer m_renderer;

    std::unique_ptr<Ghost::GhostDescriptorManager> m_descriptorManager = nullptr;

  private:
    bool m_isRunning = true;
};
