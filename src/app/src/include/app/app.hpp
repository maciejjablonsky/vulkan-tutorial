#pragma once

#include <memory>
#include <rendering/device.hpp>
#include <rendering/pipeline.hpp>
#include <rendering/swap_chain.hpp>
#include <rendering/window.hpp>
#include <vector>

namespace lve
{
class FirstApp
{
  public:
    static constexpr int WIDTH  = 800;
    static constexpr int HEIGHT = 600;
    void run();

    FirstApp();
    ~FirstApp();

  private:
    void create_pipeline_layout();
    void create_pipeline();
    void create_command_buffers();
    void draw_frame();

    LveWindow window_{WIDTH, HEIGHT, "Hello Vulkan!"};
    LveDevice device_{window_};
    LveSwapChain swap_chain_{device_, window_.get_extent()};
    std::unique_ptr<LvePipeline> pipeline_;
    VkPipelineLayout pipeline_layout_;
    std::pmr::vector<VkCommandBuffer> command_buffer_;

    // LvePipeline pipeline_{device_,
    //                      "../../../shaders/simple_shader.vert.spv",
    //                      "../../../shaders/simple_shader.frag.spv",
    //                      PipelineConfigInfo::create_default(WIDTH, HEIGHT)};
};
} // namespace lve

static_assert(!std::is_copy_constructible_v<lve::FirstApp>);
static_assert(!std::is_copy_assignable_v<lve::FirstApp>);
