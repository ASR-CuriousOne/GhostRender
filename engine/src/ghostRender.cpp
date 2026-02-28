#include <GLFW/glfw3.h>
#include <Ghost/ghostRender.hpp>

namespace Ghost {
GhostRender::GhostRender(): m_window() {}

GhostRender::~GhostRender() {
  
}

void GhostRender::run() {
  while (!glfwWindowShouldClose(m_window)) {
    glfwPollEvents();
  }
}

} // namespace Ghost
