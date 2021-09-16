#include <app/app.hpp>
#include <array>

namespace lve
{

FirstApp::FirstApp()
{
    create_pipeline_layout();
    create_pipeline();
    create_command_buffers();
}

FirstApp::~FirstApp()
{
    vkDestroyPipelineLayout(device_.device(), pipeline_layout_, nullptr);
}

void FirstApp::run()
{
    while (!window_.should_close())
    {
        glfwPollEvents();
        draw_frame();
    }

    vkDeviceWaitIdle(device_.device());
}

void FirstApp::create_pipeline_layout()
{
    VkPipelineLayoutCreateInfo pipeline_layout_info{
        .sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .setLayoutCount         = 0,
        .pSetLayouts            = nullptr,
        .pushConstantRangeCount = 0,
        .pPushConstantRanges    = nullptr};
    if (vkCreatePipelineLayout(device_.device(),
                               &pipeline_layout_info,
                               nullptr,
                               &pipeline_layout_) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create pipeline layout.");
    }
}

void FirstApp::create_pipeline()
{
    auto pipeline_config = PipelineConfigInfo::create_default(
        swap_chain_.width(), swap_chain_.height());
    pipeline_config.render_pass     = swap_chain_.getRenderPass();
    pipeline_config.pipeline_layout = pipeline_layout_;
    pipeline_ =
        std::make_unique<LvePipeline>(device_,
                                      "../../../shaders/simple_shader.vert.spv",
                                      "../../../shaders/simple_shader.frag.spv",
                                      pipeline_config);
}

void FirstApp::create_command_buffers()
{
    command_buffer_.resize(swap_chain_.imageCount());
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

    for (auto i = 0; i < command_buffer_.size(); ++i)
    {
        VkCommandBufferBeginInfo begin_info{
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
        if (vkBeginCommandBuffer(command_buffer_[i], &begin_info) != VK_SUCCESS)
        {
            throw std::runtime_error(
                "Failed to begin recording command buffer.");
        }

        VkRenderPassBeginInfo render_pass_info{
            .sType       = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
            .renderPass  = swap_chain_.getRenderPass(),
            .framebuffer = swap_chain_.getFrameBuffer(i),
            .renderArea  = {.offset = {0, 0},
                           .extent = swap_chain_.getSwapChainExtent()},
        };

        std::array<VkClearValue, 2> clear_values{};
        clear_values[0].color        = {0.1f, 0.1f, 0.1f, 1.0f};
        clear_values[1].depthStencil = {1.0f, 0};

        render_pass_info.clearValueCount =
            static_cast<uint32_t>(clear_values.size());
        render_pass_info.pClearValues = clear_values.data();

        vkCmdBeginRenderPass(
            command_buffer_[i], &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);
        pipeline_->bind(command_buffer_[i]);
        vkCmdDraw(command_buffer_[i], 3, 1, 0, 0);

        vkCmdEndRenderPass(command_buffer_[i]);
        if (vkEndCommandBuffer(command_buffer_[i]) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to record command buffer.");
        }
    }
}
void FirstApp::draw_frame()
{
    uint32_t image_index;
    auto result = swap_chain_.acquireNextImage(&image_index);
    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
    {
        throw std::runtime_error("Failed to acquire swap chain image!"); 
    }

    result = swap_chain_.submitCommandBuffers(&command_buffer_[image_index],
                                              &image_index);
    if (result != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to present swap chain image!"); 
    }
}
} // namespace lve
