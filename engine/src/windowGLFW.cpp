#include <Ghost/windowGLFW.hpp>

namespace Ghost {
WindowGLFW::WindowGLFW() {
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    m_window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);

    glfwSetWindowUserPointer(m_window, this);
    glfwSetFramebufferSizeCallback(m_window, framebufferResizeCallback);
}

WindowGLFW::~WindowGLFW() {
    glfwDestroyWindow(m_window);

    glfwTerminate();
}

void WindowGLFW::framebufferResizeCallback(GLFWwindow *window, int width,
                                           int height) {
    auto app = reinterpret_cast<WindowGLFW *>(glfwGetWindowUserPointer(window));
    app->m_framebufferResized = true;
}
} // namespace Ghost
