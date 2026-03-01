#pragma once
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>
#include <Ghost/windowGLFW.hpp>
#include <Ghost/vulkanInstance.hpp>

namespace Ghost {
class GhostRender {
private:
	WindowGLFW m_window;
	VulkanInstance m_instance;


public:
	GhostRender();
	~GhostRender();

	void run();

};
} // namespace Ghost
