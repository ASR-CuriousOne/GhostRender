#include <Ghost/windowGLFW.hpp>

namespace Ghost {
WindowGLFW::WindowGLFW() {
  glfwInit();

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  m_window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
}

WindowGLFW::~WindowGLFW() {
  glfwDestroyWindow(m_window);

  glfwTerminate();
}
} // namespace Ghost
