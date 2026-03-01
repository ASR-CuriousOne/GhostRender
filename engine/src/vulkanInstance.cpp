#include <GLFW/glfw3.h>
#include <Ghost/vulkanInstance.hpp>
#include <cstring>
#include <iostream>
#include <stdexcept>

namespace Ghost {
VulkanInstance::VulkanInstance() : m_context(), m_instance(nullptr) {

  vk::ApplicationInfo appInfo("GhostRenderEngine", vk::makeVersion(1, 0, 0),
                              "NoEngine", vk::makeVersion(1, 0, 0),
                              vk::ApiVersion10);

  std::vector<const char *> requiredExtensions =
      checkAndReturnRequiredExtensions();

  vk::InstanceCreateInfo createInfo(
      {}, &appInfo, 0, nullptr,
      static_cast<uint32_t>(requiredExtensions.size()),
      requiredExtensions.data());

  m_instance = vk::raii::Instance(m_context, createInfo);
}

std::vector<const char *> VulkanInstance::checkAndReturnRequiredExtensions() {
  std::vector<vk::ExtensionProperties> supportedExtensions =
      m_context.enumerateInstanceExtensionProperties();

  uint32_t glfwExtensionCount = 0;
  const char **glfwExtensions;

  glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

  std::vector<const char *> requiredExtensions(
      glfwExtensions, glfwExtensions + glfwExtensionCount);

  for (const char *requiredExt : requiredExtensions) {
    bool extensionFound = false;

    for (const auto &supportedExt : supportedExtensions) {
      if (std::strcmp(requiredExt, supportedExt.extensionName) == 0) {
        extensionFound = true;
        break;
      }
    }

    if (!extensionFound) {
      throw std::runtime_error(
          std::string("Missing required Vulkan extension: ") + requiredExt);
    }
  }
  std::clog << "All GLFW required Vulkan extensions are supported\n";
  return requiredExtensions;
}

VulkanInstance::~VulkanInstance() {}
} // namespace Ghost
