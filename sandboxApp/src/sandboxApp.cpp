#include "sandboxApp.hpp"
#include "imgui.h"
#include "vulkan/vulkan.hpp"
#include <Ghost/Resources/ghostDescriptors.hpp>
#include <Ghost/Resources/hdriTexture.hpp>
#include <Ghost/Resources/skyboxMaterial.hpp>
#include <Ghost/Utils/pipelineConfig.hpp>
#include <Ghost/Utils/utils.hpp>
#include <csignal>
#include <iostream>

SandboxApp::SandboxApp() {}

void SandboxApp::onInit() {
    std::cout << "SandboxApp: Initializing..." << std::endl;

    m_imguiLayer = std::make_unique<ImGuiLayer>(m_engine->getDevice(), m_window,
                                                m_engine->getRenderPass(),
                                                *(m_engine->getInstance()));

    m_assetManager =
        std::make_unique<Ghost::AssetManager>(m_engine->getDevice());

    initDescriptors();
    initPBRPipeline();
    initSkyboxPipeline();
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

    float frameTimeMs = dt * 1000.0f;

    m_frameTimes[m_frameTimeIndex] = frameTimeMs;
    m_frameTimeIndex = (m_frameTimeIndex + 1) % FRAME_HISTORY_COUNT;

    m_averageLatency = 0.0f;
    m_maxLatency = 0.0f;
    m_minLatency = 999.0f;
    for (float t : m_frameTimes) {
        m_averageLatency += t;
        if (t > m_maxLatency)
            m_maxLatency = t;
        if (t < m_minLatency && t > 0.0f)
            m_minLatency = t;
    }
    m_averageLatency /= FRAME_HISTORY_COUNT;

    float fps = (m_averageLatency > 0.0f) ? (1000.0f / m_averageLatency) : 0.0f;

    m_imguiLayer->beginFrame();

	ImGui::Begin("Engine Diagnostics");

    ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Performance Metrics");
    ImGui::Separator();

    ImGui::Text("FPS: %.1f", fps);
    ImGui::Text("Avg Latency: %.3f ms", m_averageLatency);
    ImGui::Text("Min Latency: %.3f ms", m_minLatency);
    ImGui::Text("Max Latency (Spike): %.3f ms", m_maxLatency);

    ImGui::Spacing();

    char overlay[32];
    snprintf(overlay, sizeof(overlay), "Inst: %.2f ms", frameTimeMs);
    ImGui::PlotLines("##FrameTimes", m_frameTimes.data(), m_frameTimes.size(),
                     m_frameTimeIndex, overlay, 0.0f, 10.0f, ImVec2(0, 80));

    ImGui::End();

    float aspect = m_engine->getAspectRatio();
    float fov = glm::radians(60.0f);
    float nearPlane = 1.0f;
    float farPlane = 10000.0f;

    m_camera.setPerspectiveProjection(fov, aspect, nearPlane, farPlane);

    ImGuiIO &io = ImGui::GetIO();
    bool mouseCaptured = io.WantCaptureMouse;
    bool keyboardCaptured = io.WantCaptureKeyboard;

    if (!keyboardCaptured) {
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
                    m_cameraController->setMode(
                        CameraController::Mode::FreeRoam);
                }
                oKeyPressed = true;
            }
        } else {
            oKeyPressed = false;
        }
    }

    if (!mouseCaptured && !keyboardCaptured) {
        m_cameraController->update(dt);
    }

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

    m_imguiLayer->render(frameInfo.commandBuffer);
}

void SandboxApp::onShutdown() {
    std::cout << "SandboxApp: Shutting down..." << std::endl;
}

void SandboxApp::initDescriptors() {
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
}
void SandboxApp::initPBRPipeline() {
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
void SandboxApp::initSkyboxPipeline() {
    m_descriptorManager->registerLayout(
        "SkyboxLayout",
        Ghost::GhostDescriptorSetLayout::Builder(m_engine->getDevice())
            .addBinding(0, vk::DescriptorType::eCombinedImageSampler,
                        vk::ShaderStageFlagBits::eFragment)
            .build());

    std::vector<vk::DescriptorSetLayout> skyboxSetLayouts = {
        m_descriptorManager->getLayout("global").getDescriptorSetLayout(),
        m_descriptorManager->getLayout("SkyboxLayout")
            .getDescriptorSetLayout()};
    vk::PushConstantRange pushConstantRange{};
    pushConstantRange.stageFlags =
        vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment;
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(Ghost::PBRPushConstants);

    vk::PipelineLayoutCreateInfo skyboxLayoutInfo{};
    skyboxLayoutInfo.setSetLayouts(skyboxSetLayouts);
    skyboxLayoutInfo.setPushConstantRanges(pushConstantRange);
    m_skyboxPipelineLayout =
        vk::raii::PipelineLayout(m_engine->getDevice(), skyboxLayoutInfo);

    Ghost::PipelineConfigInfo skyboxConfig;
    Ghost::PipelineConfigInfo::defaultConfig(skyboxConfig);

    skyboxConfig.depthStencilInfo.setDepthCompareOp(
        vk::CompareOp::eLessOrEqual);
    skyboxConfig.depthStencilInfo.setDepthWriteEnable(VK_FALSE);

    skyboxConfig.rasterizationInfo.setCullMode(vk::CullModeFlagBits::eFront);

    skyboxConfig.renderPass = m_engine->getRenderPass();
    skyboxConfig.pipelineLayout = *m_skyboxPipelineLayout;

    auto bindingDescriptions = Ghost::StandardVertex::getBindingDescriptions();
    auto attributeDescriptions =
        Ghost::StandardVertex::getAttributeDescriptions();
    skyboxConfig.vertexInputInfo.setVertexBindingDescriptions(
        bindingDescriptions);
    skyboxConfig.vertexInputInfo.setVertexAttributeDescriptions(
        attributeDescriptions);

    auto skyVertCode = Ghost::Utils::readFile("shaders/skybox.vert.spv");
    auto skyFragCode = Ghost::Utils::readFile("shaders/skybox.frag.spv");

    m_skyboxPipeline = std::make_shared<Ghost::GhostGraphicsPipeline>(
        m_engine->getDevice(), skyVertCode, skyFragCode, skyboxConfig);

    auto skyboxObj = Ghost::GhostGameObject::createGameObject();

    skyboxObj.transform.scale = {500.0f, 500.0f, 500.0f};

    skyboxObj.mesh = m_assetManager->getMesh("assets/meshes/cube.obj");
    auto hdriTexture = std::make_shared<Ghost::HDRITexture>(
        m_engine->getDevice(), "assets/textures/environment.hdr");

    auto skyboxMaterial = std::make_shared<Ghost::SkyboxMaterial>(
        m_engine->getDevice(), *m_descriptorManager, m_skyboxPipeline, 2);

    skyboxMaterial->setHDRIMap(hdriTexture);
    skyboxObj.material = skyboxMaterial;

    m_gameObjects.push_back(std::move(skyboxObj));
}

void SandboxApp::loadGameObjects() {
    auto env = Ghost::Utils::loadEnvFile(".env");

    auto gameObject1 = Ghost::GhostGameObject::createGameObject();
    gameObject1.transform.translation = {0.0f, 0.0f, 0.0f};
    gameObject1.transform.scale = {20.0f, 20.0f, 20.0f};

    std::string modelPath = env.contains("MODEL_PATH_1")
                                ? env["MODEL_PATH_1"]
                                : "assets/meshes/cube.obj";
    gameObject1.mesh = m_assetManager->getMesh(modelPath);

    std::string texPath = env.contains("TEXTURE_PATH_1")
                              ? env["TEXTURE_PATH_1"]
                              : "assets/textures/albedo.jpg";
    auto albedoMap = m_assetManager->getTexture<Ghost::GhostTexture>(
        "assets/textures/albedo.jpg");
    auto normalMap = m_assetManager->getTexture<Ghost::GhostTexture>(
        "assets/textures/normal.png", vk::Format::eR8G8B8A8Unorm);

    auto material = std::make_shared<Ghost::PBRMaterial>(
        m_engine->getDevice(), *m_descriptorManager, m_pbrPipeline, 1);

    material->setAlbedoMap(albedoMap);
    material->setNormalMap(normalMap);
    material->setRoughness(0.5f);
    material->setMetallic(0.1f);

    gameObject1.material = material;

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
