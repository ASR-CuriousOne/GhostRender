#include <GLFW/glfw3.h>
#include <Ghost/ghostRender.hpp>

namespace Ghost {
GhostRender::GhostRender() : m_window(), m_instance(), m_device(m_instance) {
    std::clog << m_device.getDeviceName() << std::endl;
}

GhostRender::~GhostRender() {}

void GhostRender::run() {
	std::clog << "Application Loop: Started..." << std::endl;
    while (!glfwWindowShouldClose(m_window)) {
        glfwPollEvents();
    }
	std::clog << "Application Loop: Stopped." << std::endl;
}

} // namespace Ghost
