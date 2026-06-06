#pragma once

#include "application.hpp"
#include "cameraController.hpp"
#include "ghostCamera.hpp"
#include "ghostGameObject.hpp"
#include <Ghost/Core/ghostBuffer.hpp>
#include <Ghost/Resources/hdriTexture.hpp>
#include <Ghost/Systems/hdriRenderSystem.hpp>
#include <Ghost/Systems/simpleRenderSystem.hpp>
#include <memory>
#include <vector>

struct PointLight {
    alignas(16) glm::vec4 position;
    alignas(16) glm::vec4 color;
};

struct GlobalUbo {
    alignas(16) glm::mat4 projection;
    alignas(16) glm::mat4 view;
    alignas(16) glm::vec4 ambientLightColor{1.f, 1.f, 1.f, 0.1f};
    alignas(16) glm::vec4 cameraPos;
    alignas(16) int numLights;
    PointLight lights[50];
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
    vk::raii::DescriptorSet initDescriptors();
    void updateUniformBuffer(uint32_t currentImage);

    std::unique_ptr<Ghost::HDRITexture> m_hdriTexture;

    std::vector<Ghost::GhostGameObject> m_gameObjects;

    Ghost::GhostCamera m_camera;
    std::unique_ptr<CameraController> m_cameraController;

    std::vector<std::unique_ptr<Ghost::GhostBuffer>> m_uniformBuffers;
    std::vector<vk::raii::DescriptorSet> m_descriptorSets;

    std::unique_ptr<Ghost::HDRIRenderSystem> m_hdriRenderSystem;
    std::unique_ptr<Ghost::SimpleRenderSystem> m_simpleRenderSystem;
};
