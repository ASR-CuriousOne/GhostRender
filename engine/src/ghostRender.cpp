#include <Ghost/ghostRender.hpp>

namespace Ghost {
GhostRender::GhostRender(GhostRenderConfig &config)
    : m_width(config.windowWidth), m_height(config.windowHeight) {
    m_instance = std::make_unique<VulkanInstance>(config.instanceExtensions);

    VkSurfaceKHR c_surface = config.surfaceCreateCallback(**m_instance);
    m_surface = std::make_unique<vk::raii::SurfaceKHR>(*m_instance, c_surface);

    m_device = std::make_unique<VulkanDevice>(*m_instance, *m_surface);

    m_renderer = std::make_unique<GhostRenderer>(*m_device, **m_surface,
                                                 m_width, m_height);
}

GhostRender::~GhostRender() { (*m_device)->waitIdle(); }

void GhostRender::resize(uint32_t width, uint32_t height) {
    m_width = width;
    m_height = height;
    m_isMinimized = (width == 0 || height == 0);

    if (!m_isMinimized) {
        m_renderer->flagFramebufferResized(width, height);
    }
}

float GhostRender::getAspectRatio() const {
    return static_cast<float>(m_width) / static_cast<float>(m_height);
}

vk::raii::CommandBuffer &GhostRender::beginFrame() {
    return m_renderer->beginFrame();
}

void GhostRender::endFrame() { m_renderer->endFrame(); }

void GhostRender::beginSwapChainRenderPass(
    const vk::raii::CommandBuffer &commandBuffer) {
    m_renderer->beginSwapChainRenderPass(commandBuffer);
}

void GhostRender::endSwapChainRenderPass(
    const vk::raii::CommandBuffer &commandBuffer) {
    m_renderer->endSwapChainRenderPass(commandBuffer);
}

} // namespace Ghost
