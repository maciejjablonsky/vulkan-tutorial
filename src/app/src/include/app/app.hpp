#pragma once

#include <rendering/device.hpp>
#include <rendering/pipeline.hpp>
#include <rendering/window.hpp>

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
    LveDevice device_{window_};
    LvePipeline pipeline_{device_,
                          "../../../shaders/simple_shader.vert.spv",
                          "../../../shaders/simple_shader.frag.spv",
                          PipelineConfigInfo::create_default(WIDTH, HEIGHT)};
};
} // namespace lve
