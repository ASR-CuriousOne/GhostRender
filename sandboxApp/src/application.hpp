#pragma once
#include "windowGLFW.hpp"
#include <Ghost/Resources/ghostDescriptorManager.hpp>
#include <Ghost/Systems/forwardRenderer.hpp>
#include <Ghost/Utils/frameInfo.hpp>
#include <memory>

class Application {
  public:
    Application();
    virtual ~Application() = default;

    void run();
    void close();

  protected:
    virtual void onInit() = 0;
    virtual void onUpdate(float dt) = 0;
    virtual void onRender(Ghost::FrameInfo &frameInfo) = 0;
    virtual void onShutdown() = 0;

    WindowGLFW m_window;

    std::unique_ptr<Ghost::ForwardRenderer> m_engine;

    std::unique_ptr<Ghost::GhostDescriptorManager> m_descriptorManager;

  private:
    bool m_isRunning = true;
};
