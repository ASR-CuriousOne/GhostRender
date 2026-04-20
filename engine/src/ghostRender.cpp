#include "vulkan/vulkan.hpp"
#include <GLFW/glfw3.h>
#include <Ghost/ghostRender.hpp>
#include <vulkan/vulkan_raii.hpp>

namespace Ghost {
std::atomic<bool> GhostRender::s_quitFlag{false};
GhostRender::GhostRender()
    : m_window(), m_instance(), m_surface(m_instance, m_window),
      m_device(m_instance, m_surface), m_commandPool(m_device) {

    createCommandBuffer();

    m_swapchain =
        std::make_unique<GhostSwapchain>(m_device, m_window, m_surface);

    createSyncObjects();

    vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo;
    pipelineLayoutCreateInfo.setSetLayoutCount(0)
        .setSetLayouts(nullptr)
        .setPushConstantRangeCount(0)
        .setPushConstantRanges(nullptr);

    m_pipelineLayout =
        vk::raii::PipelineLayout(m_device, pipelineLayoutCreateInfo);

    PipelineConfigInfo pipelineConfigInfo;
    PipelineConfigInfo::defaultConfig(pipelineConfigInfo);

    pipelineConfigInfo.renderPass = m_swapchain->getRenderPass();
    pipelineConfigInfo.pipelineLayout = m_pipelineLayout;

    m_graphicsPipeline = std::make_unique<GhostGraphicsPipeline>(
        m_device, c_vertShaderPath, c_fragShaderPath, pipelineConfigInfo);

    std::clog << m_device.getDeviceName() << std::endl;
}

GhostRender::~GhostRender() {}

void GhostRender::run() {
    std::clog << "Application Loop: Started..." << std::endl;
    while (!glfwWindowShouldClose(m_window) && !s_quitFlag.load()) {
        glfwPollEvents();
        drawFrame();
    }

    m_device->waitIdle();
    std::clog << "Application Loop: Stopped." << std::endl;
}

void GhostRender::createCommandBuffer() {
    vk::CommandBufferAllocateInfo allocInfo;

    allocInfo.setCommandPool(*m_commandPool)
        .setLevel(vk::CommandBufferLevel::ePrimary)
        .setCommandBufferCount(MAX_FRAMES_IN_FLIGHT);

    m_commandBuffers = vk::raii::CommandBuffers(m_device, allocInfo);
}

void GhostRender::createSyncObjects() {
    m_imageAvailableSemaphores.reserve(MAX_FRAMES_IN_FLIGHT);
    m_inFlightFences.reserve(MAX_FRAMES_IN_FLIGHT);

    vk::SemaphoreCreateInfo semaphoreInfo{};
    vk::FenceCreateInfo fenceInfo{vk::FenceCreateFlagBits::eSignaled};

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        m_imageAvailableSemaphores.emplace_back(m_device, semaphoreInfo);
        m_inFlightFences.emplace_back(m_device, fenceInfo);
    }

    size_t imageCount = m_swapchain->getImageCount();
    m_renderFinishedSemaphores.reserve(imageCount);
    for (size_t i = 0; i < imageCount; i++) {
        m_renderFinishedSemaphores.emplace_back(m_device, semaphoreInfo);
    }
}

void GhostRender::recordCommandBuffer(
    const vk::raii::CommandBuffer &commandBuffer, uint32_t imageIndex) {
    vk::CommandBufferBeginInfo beginInfo;

    commandBuffer.begin(beginInfo);

    vk::RenderPassBeginInfo renderPassInfo;
    renderPassInfo.setRenderPass(m_swapchain->getRenderPass())
        .setFramebuffer(m_swapchain->getFrameBuffer(imageIndex));
    renderPassInfo.renderArea.offset.setX(0.0f).setY(0.0f);
    renderPassInfo.renderArea.setExtent(m_swapchain->getSwapchainExtent());

    vk::ClearValue clearValue;
    clearValue.setColor({0.0f, 0.0f, 0.0f, 0.0f});

    renderPassInfo.setClearValueCount(1).setClearValues(clearValue);

    commandBuffer.beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);

    commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics,
                               **m_graphicsPipeline);

    vk::Viewport viewport;
    viewport.setX(0.0f)
        .setY(0.0f)
        .setWidth(m_swapchain->getSwapchainExtent().width)
        .setHeight(m_swapchain->getSwapchainExtent().height)
        .setMinDepth(0.0f)
        .setMaxDepth(1.0f);

    commandBuffer.setViewport(0, viewport);

    vk::Rect2D scissor;
    scissor.offset.setX(0.0f).setY(0.0f);
    scissor.setExtent(m_swapchain->getSwapchainExtent());

    commandBuffer.setScissor(0, scissor);

    commandBuffer.draw(3, 1, 0, 0);

    commandBuffer.endRenderPass();

    commandBuffer.end();
}

void GhostRender::drawFrame() {

    m_device->waitForFences({*m_inFlightFences[m_currentFrame]}, vk::True,
                            std::numeric_limits<uint64_t>::max());

    uint32_t imageIndex;
    try {
        auto resultValue = m_swapchain->aquireNextImage(
            m_imageAvailableSemaphores[m_currentFrame]);
        imageIndex = resultValue.value;
    } catch (const vk::OutOfDateKHRError &e) {
        recreateSwapchain();
        return;
    }

    m_device->resetFences({*m_inFlightFences[m_currentFrame]});

    m_commandBuffers[m_currentFrame].reset();
    recordCommandBuffer(m_commandBuffers[m_currentFrame], imageIndex);

    vk::PipelineStageFlags waitStages[] = {
        vk::PipelineStageFlagBits::eColorAttachmentOutput};

    vk::Semaphore waitSemaphores[] = {
        *m_imageAvailableSemaphores[m_currentFrame]};
    vk::Semaphore signalSemaphores[] = {
        *m_renderFinishedSemaphores[imageIndex]};

    vk::SubmitInfo submitInfo(1, waitSemaphores, waitStages, 1,
                              &(*m_commandBuffers[m_currentFrame]), 1,
                              signalSemaphores);

    m_device.submitGraphicsQueue(submitInfo, m_inFlightFences[m_currentFrame]);

    vk::SwapchainKHR swapchains[] = {**m_swapchain};
    vk::PresentInfoKHR presentInfo(1, signalSemaphores, 1, swapchains,
                                   &imageIndex);

    try {
        vk::Result result = m_device.submitPresentQueue(presentInfo);

        if (result == vk::Result::eSuboptimalKHR ||
            m_window.m_framebufferResized) {
            m_window.m_framebufferResized = false;
            recreateSwapchain();
        }
    } catch (const vk::OutOfDateKHRError &e) {
        m_window.m_framebufferResized = false;
        recreateSwapchain();
    }

    m_currentFrame = (m_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void GhostRender::recreateSwapchain() {
    int width = 0, height = 0;
    glfwGetFramebufferSize(m_window, &width, &height);
    while (width == 0 || height == 0) {
        glfwGetFramebufferSize(m_window, &width, &height);
        glfwWaitEvents();
    }

    m_device->waitIdle();

    m_graphicsPipeline.reset();
    m_swapchain.reset();

    m_swapchain =
        std::make_unique<GhostSwapchain>(m_device, m_window, m_surface);

    PipelineConfigInfo pipelineConfigInfo;
    PipelineConfigInfo::defaultConfig(pipelineConfigInfo);
    pipelineConfigInfo.renderPass = m_swapchain->getRenderPass();
    pipelineConfigInfo.pipelineLayout = m_pipelineLayout;

    m_graphicsPipeline = std::make_unique<GhostGraphicsPipeline>(
        m_device, c_vertShaderPath, c_fragShaderPath, pipelineConfigInfo);
}

} // namespace Ghost
