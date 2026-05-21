#include "windowGLFW.hpp"
#include <iostream>

WindowGLFW::WindowGLFW() {
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    m_window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);

	if (!m_window) {
        const char* description;
        int code = glfwGetError(&description);
        std::cerr << "Failed to create GLFW window! Error Code: " << code 
                  << " Description: " << (description ? description : "Unknown") << std::endl;
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window");
    }

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
