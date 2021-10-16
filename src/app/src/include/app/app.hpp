#pragma once

#include <memory>
#include <rendering/device.hpp>
#include <rendering/game_object.hpp>
#include <rendering/model.hpp>
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
    void load_game_objects();
    void create_pipeline_layout();
    void create_pipeline();
    void create_command_buffers();
    void free_command_buffers();
    void draw_frame();
    void recreate_swap_chain();
    void record_command_buffer(int image_index);
    void render_game_objects(VkCommandBuffer command_buffer);

    LveWindow window_{WIDTH, HEIGHT, "Hello Vulkan!"};
    LveDevice device_{window_};
    std::unique_ptr<LveSwapChain> swap_chain_;
    // LveSwapChain swap_chain_{device_, window_.get_extent()};
    std::unique_ptr<LvePipeline> pipeline_;
    VkPipelineLayout pipeline_layout_;
    std::pmr::vector<VkCommandBuffer> command_buffer_;
    std::vector<LveGameObject> game_objects_;
};
} // namespace lve

static_assert(!std::is_copy_constructible_v<lve::FirstApp>);
static_assert(!std::is_copy_assignable_v<lve::FirstApp>);
