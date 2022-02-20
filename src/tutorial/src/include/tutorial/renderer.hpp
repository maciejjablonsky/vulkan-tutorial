#pragma once

#include <cassert>
#include <memory>
#include <tutorial/device.hpp>
#include <tutorial/model.hpp>
#include <tutorial/swap_chain.hpp>
#include <tutorial/window.hpp>
#include <vector>

namespace lve
{
class LveRenderer
{
  public:
    LveRenderer(LveWindow& window, LveDevice& device);
    ~LveRenderer();

    VkCommandBuffer begin_frame();
    void end_frame();

    void begin_swap_chain_render_pass(VkCommandBuffer command_buffer);
    void end_swap_chain_render_pass(VkCommandBuffer command_buffer);

    VkRenderPass get_swap_chain_render_pass() const
    {

        return swap_chain_->getRenderPass();
    }
    bool is_frame_in_progress() const
    {
        return is_frame_started_;
    }

    float get_aspect_ratio() const
    {
        return swap_chain_->extentAspectRatio();
    }

    VkCommandBuffer get_current_command_buffer() const
    {
        assert(is_frame_started_ &&
               "Cannot get command buffer when frame not in progress");
        return command_buffer_[get_frame_index()];
    }

    int get_frame_index() const
    {
        assert(is_frame_in_progress() &&
               "Cannot get frame index when frame not in progress");
        return current_frame_index_;
    }

  private:
    void create_command_buffers();
    void free_command_buffers();
    void recreate_swap_chain();

    LveWindow& window_;
    LveDevice& device_;
    std::unique_ptr<LveSwapChain> swap_chain_;
    std::pmr::vector<VkCommandBuffer> command_buffer_;

    uint32_t current_image_index_;
    int current_frame_index_ = 0;
    bool is_frame_started_   = false;
};
} // namespace lve

static_assert(!std::is_copy_constructible_v<lve::LveRenderer>);
static_assert(!std::is_copy_assignable_v<lve::LveRenderer>);
