#include "application.hpp"
#include <Ghost/simpleRenderSystem.hpp>
#include <chrono>
#include <memory>

Application::Application() : m_window() {
    uint32_t glfwExtensionCount = 0;
    const char **glfwExtensions =
        glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    std::vector<const char *> extensions(glfwExtensions,
                                         glfwExtensions + glfwExtensionCount);

    Ghost::GhostRenderConfig config{};
    config.instanceExtensions = extensions;
    config.windowWidth = m_window.WIDTH;
    config.windowHeight = m_window.HEIGHT;

    config.surfaceCreateCallback = [this](VkInstance instance) {
        VkSurfaceKHR surface;
        if (glfwCreateWindowSurface(instance, m_window, nullptr, &surface) !=
            VK_SUCCESS) {
            throw std::runtime_error("Failed to create window surface!");
        }
        return surface;
    };

    m_engine = std::make_unique<Ghost::GhostRender>(config);

    m_descriptorManager =
        std::make_unique<Ghost::GhostDescriptorManager>(m_engine->getDevice());
}

void Application::run() {
    onInit();

    auto currentTime = std::chrono::high_resolution_clock::now();

    std::clog << "Application Loop: Started..." << std::endl;

    while (m_isRunning && !m_window.shouldClose()) {
        glfwPollEvents();

        if (m_window.m_framebufferResized) {
            int width = 0, height = 0;
            glfwGetFramebufferSize(m_window, &width, &height);
            while (width == 0 || height == 0) {
                glfwGetFramebufferSize(m_window, &width, &height);
                glfwWaitEvents();
            }
            m_engine->resize(width, height);
            m_window.m_framebufferResized = false;
        }

        auto newTime = std::chrono::high_resolution_clock::now();
        float frameTime =
            std::chrono::duration<float, std::chrono::seconds::period>(
                newTime - currentTime)
                .count();
        currentTime = newTime;

        onUpdate(frameTime);

        if (auto &commandBuffer = m_engine->beginFrame();
            m_engine->isFrameInProgress()) {

            Ghost::FrameInfo frameInfo{
                static_cast<uint32_t>(m_engine->getFrameIndex()), frameTime,
                commandBuffer};

            m_engine->beginSwapChainRenderPass(commandBuffer);

            onRender(frameInfo);

            m_engine->endSwapChainRenderPass(commandBuffer);
            m_engine->endFrame();
        }
    }
	m_engine->getDevice()->waitIdle();
    onShutdown();
    std::clog << "Application Loop: Stopped." << std::endl;
};

void Application::close() { m_isRunning = false; }
