#pragma once
#include <GLFW/glfw3.h>

namespace Ghost {
struct WindowGLFW {
	const uint32_t WIDTH = 800;
	const uint32_t HEIGHT = 600;

  GLFWwindow *m_window;

  WindowGLFW();
  ~WindowGLFW();

	operator GLFWwindow*() const {return m_window;}
};
} // namespace Ghost
