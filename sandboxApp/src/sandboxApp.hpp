#pragma once

#include "application.hpp"
#include <Ghost/ghostBuffer.hpp>
#include <Ghost/ghostCamera.hpp>
#include <Ghost/ghostGameObject.hpp>
#include <Ghost/simpleRenderSystem.hpp>
#include <memory>
#include <vector>

struct GlobalUbo {
    glm::mat4 projection{1.f};
    glm::mat4 view{1.f};
};

class SandboxApp : public Application {
  public:
    SandboxApp();
    ~SandboxApp() override = default;

  protected:
    void onInit() override;
    void onUpdate(float dt) override;
    void onRender(const Ghost::FrameInfo &frameInfo) override;
    void onShutdown() override;

  private:
    void loadGameObjects();
    void initDescriptors();
    void updateUniformBuffer(uint32_t currentImage);

    std::vector<Ghost::GhostGameObject> m_gameObjects;
    Ghost::GhostCamera m_camera;

    std::vector<std::unique_ptr<Ghost::GhostBuffer>> m_uniformBuffers;
    std::vector<vk::raii::DescriptorSet> m_descriptorSets;

    std::unique_ptr<Ghost::SimpleRenderSystem> m_simpleRenderSystem;
};
