#pragma once
#include "windowGLFW.hpp"
#include <Ghost/IRenderSystem.hpp>
#include <Ghost/frameInfo.hpp>
#include <Ghost/ghostDescriptorManager.hpp>
#include <Ghost/ghostRender.hpp>
#include <memory>

class Application {
  public:
    Application();
    virtual ~Application() = default;

    void run();
    void close();

    void addRenderSystem(std::unique_ptr<Ghost::IRenderSystem> system);

  protected:
    virtual void onInit() = 0;
    virtual void onUpdate(float dt) = 0;
    virtual void onRender(const Ghost::FrameInfo &frameInfo) = 0;
    virtual void onShutdown() = 0;

    WindowGLFW m_window;

    std::unique_ptr<Ghost::GhostRender> m_engine;

    std::unique_ptr<Ghost::GhostDescriptorManager> m_descriptorManager;

    std::vector<std::unique_ptr<Ghost::IRenderSystem>> m_renderSystems;

  private:
    bool m_isRunning = true;
};
