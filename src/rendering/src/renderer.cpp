
#include <algorithm>
#include <array>
#include <cmath>
#include <rendering/renderer.hpp>
#include <stdexcept>

namespace lve
{
LveRenderer::LveRenderer(LveWindow& window, LveDevice& device)
    : window_(window), device_(device)
{
    recreate_swap_chain();
    create_command_buffers();
}

LveRenderer::~LveRenderer()
{
    free_command_buffers();
}

void LveRenderer::recreate_swap_chain()
{
    auto extent = window_.get_extent();
    while (extent.width == 0 || extent.height == 0)
    {
        extent = window_.get_extent();
        glfwWaitEvents();
    }

    vkDeviceWaitIdle(device_.device());
    if (swap_chain_ == nullptr)
    {
        swap_chain_ = std::make_unique<LveSwapChain>(device_, extent);
    }
    else
    {
        std::shared_ptr<LveSwapChain> old_swap_chain = std::move(swap_chain_);
        swap_chain_ = std::make_unique<LveSwapChain>(
            device_, extent, std::move(swap_chain_));
        if (!old_swap_chain->compare_swap_formats(*swap_chain_.get()))
        {
            throw std::runtime_error(
                "Swap chain image (or depth) format has changed");
        }
    }
    // TODO
}

void LveRenderer::create_command_buffers()
{
    command_buffer_.resize(LveSwapChain::MAX_FRAMES_IN_FLIGHT);
    VkCommandBufferAllocateInfo alloc_info{
        .sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool        = device_.getCommandPool(),
        .level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = static_cast<uint32_t>(command_buffer_.size())};

    if (vkAllocateCommandBuffers(device_.device(),
                                 &alloc_info,
                                 command_buffer_.data()) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to allocate command buffers.");
    }
}

void LveRenderer::free_command_buffers()
{
    vkFreeCommandBuffers(device_.device(),
                         device_.getCommandPool(),
                         static_cast<uint32_t>(command_buffer_.size()),
                         command_buffer_.data());
    command_buffer_.clear();
}

VkCommandBuffer LveRenderer::begin_frame()
{
    assert(!is_frame_in_progress() &&
           "Can't call begin_frame() while already in progress");
    auto result = swap_chain_->acquireNextImage(&current_image_index_);

    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        recreate_swap_chain();
        return nullptr;
    }

    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
    {
        throw std::runtime_error("Failed to acquire swap chain image!");
    }

    is_frame_started_ = true;

    auto command_buffer = get_current_command_buffer();
    VkCommandBufferBeginInfo begin_info{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
    if (vkBeginCommandBuffer(command_buffer, &begin_info) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to begin recording command buffer.");
    }
    return command_buffer;
}
void LveRenderer::end_frame()
{
    assert(is_frame_in_progress() &&
           "Can't call end_frame() while frame is not in progress");
    auto command_buffer = get_current_command_buffer();
    if (vkEndCommandBuffer(command_buffer) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to record command buffer.");
    }
    auto result = swap_chain_->submitCommandBuffers(&command_buffer,
                                                    &current_image_index_);
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR ||
        window_.was_window_resized())
    {
        window_.reset_window_resized_flag();
        recreate_swap_chain();
    }
    else if (result != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to present swap chain image!");
    }
    is_frame_started_ = false;
    current_frame_index_ =
        (current_frame_index_ + 1) % LveSwapChain::MAX_FRAMES_IN_FLIGHT;
}

void LveRenderer::begin_swap_chain_render_pass(VkCommandBuffer command_buffer)
{
    assert(is_frame_in_progress() &&
           "Can't call begin_swap_chain_render_pass() if frame is not in "
           "progress");
    assert(command_buffer == get_current_command_buffer() &&
           "Can't begin reder pass on command buffer from a different frame");
    VkRenderPassBeginInfo render_pass_info{
        .sType       = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .renderPass  = swap_chain_->getRenderPass(),
        .framebuffer = swap_chain_->getFrameBuffer(current_image_index_),
        .renderArea  = {.offset = {0, 0},
                       .extent = swap_chain_->getSwapChainExtent()},
    };

    std::array<VkClearValue, 2> clear_values{};
    clear_values[0].color        = {0.1f, 0.1f, 0.1f, 1.0f};
    clear_values[1].depthStencil = {1.0f, 0};

    render_pass_info.clearValueCount =
        static_cast<uint32_t>(clear_values.size());
    render_pass_info.pClearValues = clear_values.data();

    vkCmdBeginRenderPass(
        command_buffer, &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width =
        static_cast<float>(swap_chain_->getSwapChainExtent().width);
    viewport.height =
        static_cast<float>(swap_chain_->getSwapChainExtent().height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    VkRect2D scissor{{0, 0}, swap_chain_->getSwapChainExtent()};
    vkCmdSetViewport(command_buffer, 0, 1, &viewport);
    vkCmdSetScissor(command_buffer, 0, 1, &scissor);
}

void LveRenderer::end_swap_chain_render_pass(VkCommandBuffer command_buffer)
{
    assert(
        is_frame_in_progress() &&
        "Can't call end_swap_chain_render_pass() if frame is not in progress");
    assert(command_buffer == get_current_command_buffer() &&
           "Can't end reder pass on command buffer from a different frame");
    vkCmdEndRenderPass(command_buffer);
}
} // namespace lve
