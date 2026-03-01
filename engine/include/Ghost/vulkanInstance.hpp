#pragma once
#include <vector>
#include <vulkan/vulkan_raii.hpp>

namespace Ghost {
class VulkanInstance {
private:
	vk::raii::Context m_context;   
	vk::raii::Instance m_instance;

	std::vector<const char *> checkAndReturnRequiredExtensions();

public:
	VulkanInstance();
	~VulkanInstance();

};
} // namespace Ghost
