#include <Ghost/ghostApp.hpp>
#include <Ghost/ghostModel.hpp>
#include <Ghost/utils.hpp>

namespace Ghost {
std::atomic<bool> GhostApp::s_quitFlag{false};

GhostApp::GhostApp()
    : m_window(), m_instance(), m_surface(m_instance, m_window),
      m_device(m_instance, m_surface),
      m_renderer(m_window, m_device, m_surface) {
    auto envVars = Utils::loadEnvFile(".env");

    m_vertShaderPath = envVars.contains("VERT_SHADER_PATH")
                           ? envVars["VERT_SHADER_PATH"]
                           : "./shaders/vert.spv";
    m_fragShaderPath = envVars.contains("FRAG_SHADER_PATH")
                           ? envVars["FRAG_SHADER_PATH"]
                           : "./shaders/frag.spv";

    vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo;
    pipelineLayoutCreateInfo.setSetLayoutCount(0)
        .setSetLayouts(nullptr)
        .setPushConstantRangeCount(0)
        .setPushConstantRanges(nullptr);

    const std::vector<Vertex> vertices = {{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
                                          {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
                                          {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
                                          {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}};
    const std::vector<uint32_t> indices = {0, 1, 2, 2, 3, 0};

    m_model = std::make_unique<GhostModel>(m_device, vertices, indices);

    m_pipelineLayout =
        vk::raii::PipelineLayout(m_device, pipelineLayoutCreateInfo);
    PipelineConfigInfo pipelineConfigInfo;
    PipelineConfigInfo::defaultConfig(pipelineConfigInfo);
    pipelineConfigInfo.renderPass = m_renderer.getSwapChainRenderPass();
    pipelineConfigInfo.pipelineLayout = m_pipelineLayout;

    pipelineConfigInfo.bindingDescriptions = Vertex::getBindingDescriptions();
    pipelineConfigInfo.attributeDescriptions =
        Vertex::getAttributeDescriptions();
    pipelineConfigInfo.vertexInputInfo
        .setVertexBindingDescriptions(pipelineConfigInfo.bindingDescriptions)
        .setVertexAttributeDescriptions(
            pipelineConfigInfo.attributeDescriptions);

    m_graphicsPipeline = std::make_unique<GhostGraphicsPipeline>(
        m_device, m_vertShaderPath, m_fragShaderPath, pipelineConfigInfo);

    std::clog << m_device.getDeviceName() << std::endl;
}

GhostApp::~GhostApp() {}

void GhostApp::run() {
    std::clog << "Application Loop: Started..." << std::endl;
    while (!glfwWindowShouldClose(m_window) && !s_quitFlag.load()) {
        glfwPollEvents();

        if (auto &commandBuffer = m_renderer.beginFrame();
            m_renderer.isFrameInProgress()) {
            m_renderer.beginSwapChainRenderPass(commandBuffer);

            commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics,
                                       **m_graphicsPipeline);
            m_model->bind(commandBuffer);
            m_model->draw(commandBuffer);

            m_renderer.endSwapChainRenderPass(commandBuffer);
            m_renderer.endFrame();
        }
    }
    m_device->waitIdle();

    std::clog << "Application Loop: Stopped." << std::endl;
}
} // namespace Ghost
