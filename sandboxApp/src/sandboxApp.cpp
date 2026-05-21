#include "sandboxApp.hpp"
#include <Ghost/ghostDescriptors.hpp>
#include <Ghost/utils.hpp>
#include <csignal>
#include <iostream>

SandboxApp::SandboxApp() {}

void SandboxApp::onInit() {
    std::cout << "SandboxApp: Initializing..." << std::endl;

    initDescriptors();

    loadGameObjects();

    m_simpleRenderSystem = std::make_unique<Ghost::SimpleRenderSystem>(
        m_engine->getDevice(), m_engine->getRenderPass(),
        m_descriptorManager->getLayout("global").getDescriptorSetLayout(),
        m_descriptorManager->getLayout("texture").getDescriptorSetLayout());

    m_camera.setViewTarget({5.0f, 5.0f, 2.5f}, {0.0f, 0.0f, 0.0f});
}

extern volatile sig_atomic_t g_quitRequested;

void SandboxApp::onUpdate(float dt) {

    if (glfwGetKey(m_window.m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS ||
        g_quitRequested == 1) {
        close();
    }

    float aspect = m_engine->getAspectRatio();
    m_camera.setPerspectiveProjection(glm::radians(45.0f), aspect, 0.1f,
                                      100.0f);
    m_camera.update(dt);

    for (auto &obj : m_gameObjects) {
        obj.update(dt);
    }
}

void SandboxApp::onRender(const Ghost::FrameInfo &frameInfo) {
    updateUniformBuffer(frameInfo.frameIndex);

    std::vector<Ghost::GhostRenderObject> renderObjects;

    renderObjects.reserve(m_gameObjects.size());

    for (const auto &obj : m_gameObjects) {
        if (obj.model) {
            Ghost::GhostRenderObject renderObj{};

            renderObj.transformMatrix = obj.transform.mat4();
            renderObj.model = obj.model;

            if (*obj.textureDescriptorSet) {
                renderObj.textureDescriptorSet = *obj.textureDescriptorSet;
            }

            renderObjects.push_back(renderObj);
        }
    }

    m_simpleRenderSystem->renderGameObjects(
        frameInfo, renderObjects, m_descriptorSets[frameInfo.frameIndex]);
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
                        vk::ShaderStageFlagBits::eVertex)
            .build());

    m_descriptorManager->registerLayout(
        "texture",
        Ghost::GhostDescriptorSetLayout::Builder(m_engine->getDevice())
            .addBinding(0, vk::DescriptorType::eCombinedImageSampler,
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

void SandboxApp::loadGameObjects() {
    auto env = Ghost::Utils::loadEnvFile(".env");

    auto texSets = m_descriptorManager->allocateSets("texture", 2);

    auto model1 = Ghost::GhostModel::createModelFromFile(m_engine->getDevice(),
                                                         env["MODEL_PATH_1"]);

    std::string texPath1 = env.contains("TEXTURE_PATH_1")
                               ? env["TEXTURE_PATH_1"]
                               : "assets/textures/texture1.jpg";
    auto texture1 =
        std::make_shared<Ghost::GhostTexture>(m_engine->getDevice(), texPath1);

    auto gameObject1 = Ghost::GhostGameObject::createGameObject();
    gameObject1.model = model1;
    gameObject1.texture = texture1;
    gameObject1.transform.translation = {-2.0f, 0.0f, 0.0f};
    gameObject1.transform.scale = {1.0f, 1.0f, 1.0f};

    gameObject1.textureDescriptorSet = std::move(texSets[0]);

    auto imageInfo1 = texture1->descriptorInfo();
    Ghost::GhostDescriptorWriter(m_descriptorManager->getLayout("texture"))
        .writeImage(0, &imageInfo1)
        .build(gameObject1.textureDescriptorSet, m_engine->getDevice());

    m_gameObjects.push_back(std::move(gameObject1));

    auto model2 = Ghost::GhostModel::createModelFromFile(m_engine->getDevice(),
                                                         env["MODEL_PATH_2"]);

    std::string texPath2 = env.contains("TEXTURE_PATH_2")
                               ? env["TEXTURE_PATH_2"]
                               : "assets/textures/texture2.jpg";
    auto texture2 =
        std::make_shared<Ghost::GhostTexture>(m_engine->getDevice(), texPath2);

    auto gameObject2 = Ghost::GhostGameObject::createGameObject();
    gameObject2.model = model2;
    gameObject2.texture = texture2;
    gameObject2.transform.translation = {2.0f, 0.0f, 0.0f};
    gameObject2.transform.scale = {1.0f, 1.0f, 1.0f};

    gameObject2.textureDescriptorSet = std::move(texSets[1]);

    auto imageInfo2 = texture1->descriptorInfo();
    Ghost::GhostDescriptorWriter(m_descriptorManager->getLayout("texture"))
        .writeImage(0, &imageInfo2)
        .build(gameObject2.textureDescriptorSet, m_engine->getDevice());

    m_gameObjects.push_back(std::move(gameObject2));
}

void SandboxApp::updateUniformBuffer(uint32_t currentImage) {
    GlobalUbo ubo{};
    ubo.projection = m_camera.getProjection();
    ubo.view = m_camera.getView();

    m_uniformBuffers[currentImage]->writeToBuffer(
        std::span<const GlobalUbo>(&ubo, 1));
}
