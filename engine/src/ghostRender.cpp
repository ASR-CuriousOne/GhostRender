#include <GLFW/glfw3.h>
#include <Ghost/ghostRender.hpp>

namespace Ghost {
std::atomic<bool> GhostRender::s_quitFlag{false};
GhostRender::GhostRender()
    : m_window(), m_instance(), m_surface(m_instance, m_window),
      m_device(m_instance, m_surface) {

    m_swapchain =
        std::make_unique<GhostSwapchain>(m_device, m_window, m_surface);

    std::clog << m_device.getDeviceName() << std::endl;
}

GhostRender::~GhostRender() {}

void GhostRender::run() {
    std::clog << "Application Loop: Started..." << std::endl;
    while (!glfwWindowShouldClose(m_window) && !s_quitFlag.load()) {
        glfwPollEvents();
    }
    std::clog << "Application Loop: Stopped." << std::endl;
}

} // namespace Ghost
