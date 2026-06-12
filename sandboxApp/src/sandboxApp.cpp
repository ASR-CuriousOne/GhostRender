#include "sandboxApp.hpp"
#include <Ghost/Resources/ghostDescriptors.hpp>
#include <Ghost/Utils/pipelineConfig.hpp>
#include <Ghost/Utils/utils.hpp>
#include <csignal>
#include <iostream>

SandboxApp::SandboxApp() {}

void SandboxApp::onInit() {
    std::cout << "SandboxApp: Initializing..." << std::endl;

    m_assetManager =
        std::make_unique<Ghost::AssetManager>(m_engine->getDevice());

    initDescriptorsAndPipelines();

    loadGameObjects();

    m_cameraController = std::make_unique<CameraController>(m_camera, m_window);
    m_cameraController->setMode(CameraController::Mode::Orbit);
    m_cameraController->setTarget(m_gameObjects[0].transform.translation);
}

extern volatile sig_atomic_t g_quitRequested;

void SandboxApp::onUpdate(float dt) {

    if (glfwGetKey(m_window.m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS ||
        g_quitRequested == 1) {
        close();
    }

    float aspect = m_engine->getAspectRatio();
    float fov = glm::radians(60.0f);
    float nearPlane = 1.0f;
    float farPlane = 10000.0f;

    m_camera.setPerspectiveProjection(fov, aspect, nearPlane, farPlane);

    static bool oKeyPressed = false;
    GLFWwindow *window = m_window.getWindow();
    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) {
        if (!oKeyPressed) {
            if (m_cameraController->getMode() ==
                CameraController::Mode::FreeRoam) {
                m_cameraController->setMode(CameraController::Mode::Orbit);
                if (!m_gameObjects.empty()) {
                    m_cameraController->setTarget(
                        m_gameObjects[0].transform.translation);
                }
            } else {
                m_cameraController->setMode(CameraController::Mode::FreeRoam);
            }
            oKeyPressed = true;
        }
    } else {
        oKeyPressed = false;
    }

    m_cameraController->update(dt);

    for (auto &obj : m_gameObjects) {
        obj.update(dt);
    }
}

void SandboxApp::onRender(Ghost::FrameInfo &frameInfo) {
    updateUniformBuffer(frameInfo.frameIndex);

    frameInfo.commandBuffer.bindDescriptorSets(
        vk::PipelineBindPoint::eGraphics, m_pbrPipeline->getPipelineLayout(), 0,
        {*m_descriptorSets[frameInfo.frameIndex]}, nullptr);

    std::vector<Ghost::GhostRenderObject> renderObjects;
    renderObjects.reserve(m_gameObjects.size());

    for (auto &obj : m_gameObjects) {
        if (obj.mesh && obj.material) {
            Ghost::GhostRenderObject renderObj{};
            renderObj.transformMatrix = obj.transform.mat4();
            renderObj.mesh = obj.mesh;
            renderObj.material = obj.material;

            renderObjects.push_back(renderObj);
        }
    }

    m_engine->renderScene(frameInfo.commandBuffer, renderObjects);
}

void SandboxApp::onShutdown() {
    std::cout << "SandboxApp: Shutting down..." << std::endl;
}

void SandboxApp::initDescriptorsAndPipelines() {
    for (int i = 0; i < Ghost::MAX_FRAMES_IN_FLIGHT; i++) {
        m_uniformBuffers.push_back(std::make_unique<Ghost::GhostBuffer>(
            m_engine->getDevice(), sizeof(GlobalUbo),
            vk::BufferUsageFlagBits::eUniformBuffer,
            vk::MemoryPropertyFlagBits::eHostVisible |
                vk::MemoryPropertyFlagBits::eHostCoherent));
        m_uniformBuffers[i]->map();
    }

    m_descriptorManager->registerLayout(
        "global",
        Ghost::GhostDescriptorSetLayout::Builder(m_engine->getDevice())
            .addBinding(0, vk::DescriptorType::eUniformBuffer,
                        vk::ShaderStageFlagBits::eVertex |
                            vk::ShaderStageFlagBits::eFragment)
            .build());

    m_descriptorSets = m_descriptorManager->allocateSets(
        "global", Ghost::MAX_FRAMES_IN_FLIGHT);
    for (int i = 0; i < Ghost::MAX_FRAMES_IN_FLIGHT; i++) {
        auto bufferInfo = m_uniformBuffers[i]->descriptorInfo();
        Ghost::GhostDescriptorWriter(m_descriptorManager->getLayout("global"))
            .writeBuffer(0, &bufferInfo)
            .build(m_descriptorSets[i], m_engine->getDevice());
    }

    m_descriptorManager->registerLayout(
        "PBRMaterialLayout",
        Ghost::GhostDescriptorSetLayout::Builder(m_engine->getDevice())
            .addBinding(0, vk::DescriptorType::eCombinedImageSampler,
                        vk::ShaderStageFlagBits::eFragment)
            .addBinding(1, vk::DescriptorType::eCombinedImageSampler,
                        vk::ShaderStageFlagBits::eFragment)
            .build());

    vk::PushConstantRange pushConstantRange{};
    pushConstantRange.stageFlags =
        vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment;
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(Ghost::PBRPushConstants);

    std::vector<vk::DescriptorSetLayout> descriptorSetLayouts = {
        m_descriptorManager->getLayout("global").getDescriptorSetLayout(),
        m_descriptorManager->getLayout("PBRMaterialLayout")
            .getDescriptorSetLayout()};

    vk::PipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.setSetLayouts(descriptorSetLayouts);
    pipelineLayoutInfo.setPushConstantRanges(pushConstantRange);

    m_pbrPipelineLayout =
        vk::raii::PipelineLayout(m_engine->getDevice(), pipelineLayoutInfo);

    Ghost::PipelineConfigInfo configInfo;

    Ghost::PipelineConfigInfo::defaultConfig(configInfo);

    configInfo.renderPass = m_engine->getRenderPass();
    configInfo.pipelineLayout = *m_pbrPipelineLayout;

    auto bindingDescriptions = Ghost::StandardVertex::getBindingDescriptions();
    auto attributeDescriptions =
        Ghost::StandardVertex::getAttributeDescriptions();

    configInfo.vertexInputInfo.setVertexBindingDescriptions(
        bindingDescriptions);
    configInfo.vertexInputInfo.setVertexAttributeDescriptions(
        attributeDescriptions);

    auto vertCode = Ghost::Utils::readFile("shaders/pbr.vert.spv");
    auto fragCode = Ghost::Utils::readFile("shaders/pbr.frag.spv");

    m_pbrPipeline = std::make_shared<Ghost::GhostGraphicsPipeline>(
        m_engine->getDevice(), vertCode, fragCode, configInfo);
}

void SandboxApp::loadGameObjects() {
    auto env = Ghost::Utils::loadEnvFile(".env");

    auto gameObject1 = Ghost::GhostGameObject::createGameObject();
    gameObject1.transform.translation = {0.0f, 0.0f, 0.0f};
    gameObject1.transform.scale = {20.0f, 20.0f, 20.0f};

    std::string modelPath = env.contains("MODEL_PATH_1")
                                ? env["MODEL_PATH_1"]
                                : "assets/models/cube.obj";
    gameObject1.mesh = m_assetManager->getMesh(modelPath);

    std::string texPath = env.contains("TEXTURE_PATH_1")
                              ? env["TEXTURE_PATH_1"]
                              : "assets/textures/albedo.jpg";
    auto albedoMap = m_assetManager->getTexture<Ghost::GhostTexture>(
        "assets/textures/albedo.jpg");
    auto normalMap = m_assetManager->getTexture<Ghost::GhostTexture>(
        "assets/textures/normal.png", vk::Format::eR8G8B8A8Unorm);

    auto myMaterial = std::make_shared<Ghost::PBRMaterial>(
        m_engine->getDevice(), *m_descriptorManager, m_pbrPipeline, 1);

    myMaterial->setAlbedoMap(albedoMap);
    myMaterial->setNormalMap(normalMap);
    myMaterial->setRoughness(0.5f);
    myMaterial->setMetallic(0.1f);

    gameObject1.material = myMaterial;

    m_gameObjects.push_back(std::move(gameObject1));
}

void SandboxApp::updateUniformBuffer(uint32_t currentImage) {
    PointLight light1 = {.position = {0.0f, 200.0f, 0.0f, 100.0f},
                         .color = {1.0f, 1.0f, 0.95f, 160000.0f}};
    PointLight light2 = {.position = {0.0f, -2.0f, 2.0f, 10.0f},
                         .color = {0.0f, 1.0f, 1.0f, 10.0f}};
    GlobalUbo ubo{};
    ubo.projection = m_camera.getProjection();
    ubo.view = m_camera.getView();
    ubo.cameraPos = glm::vec4(m_camera.getPosition(), 1.0f);
    ubo.ambientLightColor = {0.1f, 0.1f, 0.1f, 0.1f};
    ubo.numLights = 2;
    ubo.lights[0] = light1;
    ubo.lights[1] = light2;

    m_uniformBuffers[currentImage]->writeToBuffer(
        std::span<const GlobalUbo>(&ubo, 1));
}
