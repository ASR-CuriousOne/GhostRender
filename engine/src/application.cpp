#include <Ghost/application.hpp>
#include <Ghost/simpleRenderSystem.hpp>
#include <chrono>

namespace Ghost {
Application::Application()
    : m_window(), m_instance(), m_surface(m_instance, m_window),
      m_device(m_instance, m_surface),
      m_renderer(m_window, m_device, m_surface) {}

void Application::run() {
    onInit();

    auto currentTime = std::chrono::high_resolution_clock::now();

    std::clog << "Application Loop: Started..." << std::endl;

    while (m_isRunning && !m_window.shouldClose()) {
        glfwPollEvents();

        auto newTime = std::chrono::high_resolution_clock::now();
        float frameTime =
            std::chrono::duration<float, std::chrono::seconds::period>(
                newTime - currentTime)
                .count();
        currentTime = newTime;

        onUpdate(frameTime);

        if (auto &commandBuffer = m_renderer.beginFrame();
            m_renderer.isFrameInProgress()) {

            FrameInfo frameInfo{
                static_cast<uint32_t>(m_renderer.getFrameIndex()), frameTime,
                commandBuffer};

            m_renderer.beginSwapChainRenderPass(commandBuffer);

            onRender(frameInfo);

            m_renderer.endSwapChainRenderPass(commandBuffer);
            m_renderer.endFrame();
        }
    }

    m_device->waitIdle();

    std::clog << "Application Loop: Stopped." << std::endl;
};

void Ghost::Application::close() {
    m_isRunning = false;
}


} // namespace Ghost
