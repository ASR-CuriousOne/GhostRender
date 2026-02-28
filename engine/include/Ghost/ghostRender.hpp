#pragma once
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>
#include <Ghost/windowGLFW.hpp>

namespace Ghost {
class GhostRender {
private:
	WindowGLFW m_window;

public:
	GhostRender();
	~GhostRender();

	void run();

};
} // namespace Ghost
