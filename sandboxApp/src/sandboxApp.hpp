#pragma once

#include "application.hpp"
#include "cameraController.hpp"
#include "ghostCamera.hpp"
#include "ghostGameObject.hpp"

#include <Ghost/Core/ghostGraphicsPipeline.hpp>
#include <Ghost/Resources/assetManager.hpp>
#include <Ghost/Resources/pbrMaterial.hpp>
#include <Ghost/Systems/forwardRenderer.hpp>

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
    void onRender(Ghost::FrameInfo &frameInfo) override;
    void onShutdown() override;

  private:
    void initDescriptors();
    void initPBRPipeline();
    void initSkyboxPipeline();
    void loadGameObjects();

    void updateUniformBuffer(uint32_t currentImage);

    std::unique_ptr<Ghost::AssetManager> m_assetManager;

    vk::raii::PipelineLayout m_skyboxPipelineLayout = nullptr;
    std::shared_ptr<Ghost::GhostGraphicsPipeline> m_skyboxPipeline;

    vk::raii::PipelineLayout m_pbrPipelineLayout = nullptr;
    std::shared_ptr<Ghost::GhostGraphicsPipeline> m_pbrPipeline;

    std::vector<std::unique_ptr<Ghost::GhostBuffer>> m_uniformBuffers;
    std::vector<vk::raii::DescriptorSet> m_descriptorSets;

    std::vector<Ghost::GhostGameObject> m_gameObjects;
    Ghost::GhostCamera m_camera;
    std::unique_ptr<CameraController> m_cameraController;
};
