#include <Ghost/ghostApp.hpp>
#include <Ghost/utils.hpp>
#include <memory>

namespace Ghost {
struct PushConstantData {
    glm::mat4 model{1.f};
};

std::atomic<bool> GhostApp::s_quitFlag{false};

GhostApp::GhostApp()
    : m_window(), m_instance(), m_surface(m_instance, m_window),
      m_device(m_instance, m_surface),
      m_renderer(m_window, m_device, m_surface) {

    initDescriptors();

    loadGameObjects();

    std::clog << m_device.getDeviceName() << std::endl;
}

void GhostApp::initDescriptors() {
    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        m_uniformBuffers.push_back(std::make_unique<GhostBuffer>(
            m_device, sizeof(Ubo), vk::BufferUsageFlagBits::eUniformBuffer,
            vk::MemoryPropertyFlagBits::eHostVisible |
                vk::MemoryPropertyFlagBits::eHostCoherent));
        m_uniformBuffers[i]->map();
    }

    m_globalSetLayout = GhostDescriptorSetLayout::Builder(m_device)
                            .addBinding(0, vk::DescriptorType::eUniformBuffer,
                                        vk::ShaderStageFlagBits::eVertex)
                            .build();

    m_textureSetLayout =
        GhostDescriptorSetLayout::Builder(m_device)
            .addBinding(0, vk::DescriptorType::eCombinedImageSampler,
                        vk::ShaderStageFlagBits::eFragment)
            .build();

    m_globalPool =
        GhostDescriptorPool::Builder(m_device)
            .setMaxSets(MAX_FRAMES_IN_FLIGHT + 50)
            .addPoolSize(vk::DescriptorType::eUniformBuffer,
                         MAX_FRAMES_IN_FLIGHT)
            .addPoolSize(vk::DescriptorType::eCombinedImageSampler, 50)
            .setPoolFlags(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet)
            .build();

    m_descriptorSets = m_globalPool->allocateDescriptorSets(
        MAX_FRAMES_IN_FLIGHT, m_globalSetLayout->getDescriptorSetLayout());

    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        auto bufferInfo = m_uniformBuffers[i]->descriptorInfo();

        GhostDescriptorWriter(*m_globalSetLayout)
            .writeBuffer(0, &bufferInfo)
            .build(m_descriptorSets[i], m_device);
    }
}

void GhostApp::loadGameObjects() {
    auto env = Utils::loadEnvFile(".env");

    auto model1 =
        GhostModel::createModelFromFile(m_device, env["MODEL_PATH_1"]);

    std::string texPath1 = env.contains("TEXTURE_PATH_1")
                               ? env["TEXTURE_PATH_1"]
                               : "assets/textures/texture1.jpg";
    auto texture1 = std::make_shared<GhostTexture>(m_device, texPath1);

    auto gameObject1 = GhostGameObject::createGameObject();
    gameObject1.model = model1;
    gameObject1.texture = texture1;
    gameObject1.transform.translation = {-2.0f, 0.0f, 0.0f};
    gameObject1.transform.scale = {1.0f, 1.0f, 1.0f};

    auto texSets1 = m_globalPool->allocateDescriptorSets(
        1, m_textureSetLayout->getDescriptorSetLayout());
    gameObject1.textureDescriptorSet = std::move(texSets1[0]);

    auto imageInfo1 = texture1->descriptorInfo();
    GhostDescriptorWriter(*m_textureSetLayout)
        .writeImage(0, &imageInfo1)
        .build(gameObject1.textureDescriptorSet, m_device);

    m_gameObjects.push_back(std::move(gameObject1));

    auto model2 =
        GhostModel::createModelFromFile(m_device, env["MODEL_PATH_2"]);

    std::string texPath2 = env.contains("TEXTURE_PATH_2")
                               ? env["TEXTURE_PATH_2"]
                               : "assets/textures/texture2.jpg";
    auto texture2 = std::make_shared<GhostTexture>(m_device, texPath2);

    auto gameObject2 = GhostGameObject::createGameObject();
    gameObject2.model = model2;
    gameObject2.texture = texture2;
    gameObject2.transform.translation = {2.0f, 0.0f, 0.0f};
    gameObject2.transform.scale = {1.0f, 1.0f, 1.0f};

    auto texSets2 = m_globalPool->allocateDescriptorSets(
        1, m_textureSetLayout->getDescriptorSetLayout());
    gameObject2.textureDescriptorSet = std::move(texSets2[0]);

    auto imageInfo2 = texture2->descriptorInfo();
    GhostDescriptorWriter(*m_textureSetLayout)
        .writeImage(0, &imageInfo2)
        .build(gameObject2.textureDescriptorSet, m_device);

    m_gameObjects.push_back(std::move(gameObject2));
}

void GhostApp::updateUniformBuffer(uint32_t currentImage,
                                   const GhostCamera &camera) {
    Ubo ubo{};

    ubo.projection = camera.getProjection();
    ubo.view = camera.getView();

    m_uniformBuffers[currentImage]->writeToBuffer(
        std::span<const Ubo>(&ubo, 1));
}

GhostApp::~GhostApp() {}

void GhostApp::run() {
    std::clog << "Application Loop: Started..." << std::endl;

    GhostCamera camera{};

    camera.setViewTarget({5.0f, 5.0f, 2.5f}, {0.0f, 0.0f, 0.0f});

    auto currentTime = std::chrono::high_resolution_clock::now();

    SimpleRenderSystem simpleRenderSystem{
        m_device, m_renderer.getSwapChainRenderPass(),
        m_globalSetLayout->getDescriptorSetLayout(),
        m_textureSetLayout->getDescriptorSetLayout()};

    while (!glfwWindowShouldClose(m_window) && !s_quitFlag.load()) {
        glfwPollEvents();

        auto newTime = std::chrono::high_resolution_clock::now();
        float frameTime =
            std::chrono::duration<float, std::chrono::seconds::period>(
                newTime - currentTime)
                .count();
        currentTime = newTime;

        float aspect = (float)m_renderer.getSwapchainExtent().width /
                       (float)m_renderer.getSwapchainExtent().height;
        camera.setPerspectiveProjection(glm::radians(45.0f), aspect, 0.1f,
                                        100.0f);
        camera.update(frameTime);

        for (auto &obj : m_gameObjects) {
            obj.update(frameTime);
        }

        if (auto &commandBuffer = m_renderer.beginFrame();
            m_renderer.isFrameInProgress()) {
            int frameIndex = m_renderer.getFrameIndex();

            updateUniformBuffer(frameIndex, camera);

            m_renderer.beginSwapChainRenderPass(commandBuffer);

            simpleRenderSystem.renderGameObjects(commandBuffer, m_gameObjects,
                                                 m_descriptorSets[frameIndex]);

            m_renderer.endSwapChainRenderPass(commandBuffer);
            m_renderer.endFrame();
        }
    }
    m_device->waitIdle();

    std::clog << "Application Loop: Stopped." << std::endl;
}
} // namespace Ghost
