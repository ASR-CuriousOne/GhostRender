#include <Ghost/ghostApp.hpp>
#include <Ghost/utils.hpp>

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

    m_globalPool =
        GhostDescriptorPool::Builder(m_device)
            .setMaxSets(MAX_FRAMES_IN_FLIGHT)
            .addPoolSize(vk::DescriptorType::eUniformBuffer,
                         MAX_FRAMES_IN_FLIGHT)
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
    const std::vector<Vertex> vertices = {
        {{0.0f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}},
        {{0.5f, 0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},
        {{-0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}}};
    const std::vector<uint32_t> indices = {0, 1, 2};

    auto model = std::make_shared<GhostModel>(m_device, vertices, indices);

    auto triangle1 = GhostGameObject::createGameObject();
    triangle1.model = model;
    triangle1.transform.translation = {-0.5f, 0.0f, 0.0f};
    triangle1.transform.scale = {0.5f, 0.5f, 0.5f};

    auto triangle2 = GhostGameObject::createGameObject();
    triangle2.model = model;
    triangle2.transform.translation = {0.5f, 0.0f, 0.0f};
    triangle2.transform.scale = {0.7f, 0.7f, 0.7f};
    triangle2.transform.rotation.z = 3.14159f;

    m_gameObjects.push_back(std::move(triangle1));
    m_gameObjects.push_back(std::move(triangle2));
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

    auto currentTime = std::chrono::high_resolution_clock::now();

    SimpleRenderSystem simpleRenderSystem{
        m_device, m_renderer.getSwapChainRenderPass(),
        m_globalSetLayout->getDescriptorSetLayout()};

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
                                        10.0f);
        camera.update(frameTime);

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
