#pragma once
#include <GLFW/glfw3.h>

struct WindowGLFW {
    GLFWwindow *m_window;

    const uint32_t WIDTH = 800;
    const uint32_t HEIGHT = 600;

    bool m_framebufferResized = false;

    WindowGLFW();
    ~WindowGLFW();

    operator GLFWwindow *() const { return m_window; }

	bool shouldClose(){return glfwWindowShouldClose(m_window);}

    static void framebufferResizeCallback(GLFWwindow *window, int width,
                                          int height);
};
