#pragma once
#include <vulkan/vulkan.hpp>

namespace Ghost {
class VulkanInstance {
private:
	vk::Instance m_instance;

public:
	VulkanInstance(){
		vk::ApplicationInfo appInfo("GhostRenderEngine", VK_MAKE_VERSION(1,0,0), "NoEngine", VK_MAKE_VERSION(1, 0, 0), VK_VERSION_1_0);
	}

};
} // namespace Ghost
