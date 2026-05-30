#pragma once

#include <Ghost/frameInfo.hpp>
#include <Ghost/ghostRenderObject.hpp>
#include <vector>

namespace Ghost {

class IRenderSystem {
  public:
    virtual ~IRenderSystem() = default;

    virtual void render(const FrameInfo &frameInfo,
                        std::vector<GhostRenderObject> &renderObjects,
                        const vk::raii::DescriptorSet &globalDescriptorSet) = 0;
};

} // namespace Ghost
