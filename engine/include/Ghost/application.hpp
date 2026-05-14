#pragma once
#include <Ghost/ghostCamera.hpp>
#include <Ghost/ghostDescriptorManager.hpp>
#include <Ghost/ghostRenderObject.hpp>
#include <Ghost/ghostRenderer.hpp>
#include <Ghost/vulkanDevice.hpp>
#include <Ghost/frameInfo.hpp>

namespace Ghost {
class Application {
  public:
    Application();
    virtual ~Application() = default;

    void run();
	void close();

  protected:
    virtual void onInit() = 0;
    virtual void onUpdate(float dt) = 0;
    virtual void onRender(const FrameInfo& frameInfo) = 0;
    virtual void onShutdown() = 0;

    WindowGLFW m_window;
    VulkanInstance m_instance;
    GhostSurface m_surface;
    VulkanDevice m_device;
    GhostRenderer m_renderer;

    std::unique_ptr<GhostDescriptorManager> m_descriptorManager = nullptr;

  private:
    bool m_isRunning = true;
};
} // namespace Ghost
