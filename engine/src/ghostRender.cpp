#include <GLFW/glfw3.h>
#include <Ghost/ghostRender.hpp>

namespace Ghost {
GhostRender::GhostRender(){}

GhostRender::~GhostRender() {
  
}

void GhostRender::run() {
  while (!glfwWindowShouldClose(m_window)) {
    glfwPollEvents();
  }
}

} // namespace Ghost
