#pragma once

#include <rendering/window.hpp>
#include <rendering/pipeline.hpp>

namespace lve
{
class FirstApp
{
  public:
    static constexpr int WIDTH  = 800;
    static constexpr int HEIGHT = 600;
    void run();
  private:
    LveWindow window_{WIDTH, HEIGHT, "Hello Vulkan!"};
    LvePipeline pipeline_{"../../../shaders/simple_shader.vert.spv", "../../../shaders/simple_shader.frag.spv"};
};
} // namespace lve
