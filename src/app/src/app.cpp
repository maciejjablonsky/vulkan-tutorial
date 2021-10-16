#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <app/app.hpp>
#include <array>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace lve
{
struct SimplePushConstantData
{
    glm::mat2 transform{1.f};
    glm::vec2 offset;
    alignas(16) glm::vec3 color;
};

FirstApp::FirstApp()
{
    load_game_objects();
    create_pipeline_layout();
    recreate_swap_chain();
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

void sierpinski(std::pmr::vector<LveModel::Vertex>& vertices,
                int depth,
                glm::vec2 left,
                glm::vec2 right,
                glm::vec2 top)
{
    if (depth <= 0)
    {
        vertices.push_back({top, {1.f, 0.f, 0.f}});
        vertices.push_back({right, {0.f, 1.f, 0.f}});
        vertices.push_back({left, {0.f, 0.f, 1.f}});
    }
    else
    {
        auto left_top   = 0.5f * (left + top);
        auto right_top  = 0.5f * (right + top);
        auto left_right = 0.5f * (left + right);
        sierpinski(vertices, depth - 1, left, left_right, left_top);
        sierpinski(vertices, depth - 1, left_right, right, right_top);
        sierpinski(vertices, depth - 1, left_top, right_top, top);
    }
}

void FirstApp::load_game_objects()
{
    std::pmr::vector<LveModel::Vertex> vertices{
        {{0.0f, -0.5f}, {1.f, 0.f, 0.f}},
        {{0.5f, 0.5f}, {0.f, 1.f, 0.f}},
        {{-0.5f, 0.5f}, {0.f, 0.f, 1.f}}};
    constexpr auto depth = 5;
    // std::pmr::vector<LveModel::Vertex> vertices(3 * (std::powl(3, depth)));
    // sierpinski(vertices, depth, {-1.f, 1.f}, {1.f, 1.f}, {0.0f, -1.f});
    auto model     = std::make_shared<LveModel>(device_, vertices);
    auto triangle  = LveGameObject::create_game_object();
    triangle.model = model;
    triangle.color = {.1f, .8f, .1f};
    triangle.transform2d.translation.x = .2f;
    triangle.transform2d.scale         = {2.f, .5f};
    triangle.transform2d.rotation      = .25f * glm::two_pi<float>();
    game_objects_.push_back(std::move(triangle));
}

void FirstApp::create_pipeline_layout()
{
    VkPushConstantRange push_constant_range{};
    push_constant_range.stageFlags =
        VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    push_constant_range.offset = 0;
    push_constant_range.size   = sizeof(SimplePushConstantData);

    VkPipelineLayoutCreateInfo pipeline_layout_info{
        .sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .setLayoutCount         = 0,
        .pSetLayouts            = nullptr,
        .pushConstantRangeCount = 1,
        .pPushConstantRanges    = &push_constant_range};
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
    assert(swap_chain_ != nullptr &&
           "Cannot create pipeline before swap chain");
    assert(pipeline_layout_ != nullptr &&
           "Cannot create pipeline before pipeline layout");
    PipelineConfigInfo pipeline_config{};
    LvePipeline::default_pipeline_config_info(pipeline_config);
    pipeline_config.render_pass     = swap_chain_->getRenderPass();
    pipeline_config.pipeline_layout = pipeline_layout_;
    pipeline_ =
        std::make_unique<LvePipeline>(device_,
                                      "../../../shaders/simple_shader.vert.spv",
                                      "../../../shaders/simple_shader.frag.spv",
                                      pipeline_config);
}

void FirstApp::recreate_swap_chain()
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
        swap_chain_ = std::make_unique<LveSwapChain>(
            device_, extent, std::move(swap_chain_));
        if (swap_chain_->imageCount() != command_buffer_.size())
        {
            free_command_buffers();
            create_command_buffers();
        }
    }
    create_pipeline();
}

void FirstApp::create_command_buffers()
{
    command_buffer_.resize(swap_chain_->imageCount());
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

void FirstApp::free_command_buffers()
{
    vkFreeCommandBuffers(device_.device(),
                         device_.getCommandPool(),
                         static_cast<uint32_t>(command_buffer_.size()),
                         command_buffer_.data());
    command_buffer_.clear();
}

void FirstApp::record_command_buffer(int image_index)
{
    VkCommandBufferBeginInfo begin_info{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
    if (vkBeginCommandBuffer(command_buffer_[image_index], &begin_info) !=
        VK_SUCCESS)
    {
        throw std::runtime_error("Failed to begin recording command buffer.");
    }

    VkRenderPassBeginInfo render_pass_info{
        .sType       = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .renderPass  = swap_chain_->getRenderPass(),
        .framebuffer = swap_chain_->getFrameBuffer(image_index),
        .renderArea  = {.offset = {0, 0},
                       .extent = swap_chain_->getSwapChainExtent()},
    };

    std::array<VkClearValue, 2> clear_values{};
    clear_values[0].color        = {0.1f, 0.1f, 0.1f, 1.0f};
    clear_values[1].depthStencil = {1.0f, 0};

    render_pass_info.clearValueCount =
        static_cast<uint32_t>(clear_values.size());
    render_pass_info.pClearValues = clear_values.data();

    vkCmdBeginRenderPass(command_buffer_[image_index],
                         &render_pass_info,
                         VK_SUBPASS_CONTENTS_INLINE);

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
    vkCmdSetViewport(command_buffer_[image_index], 0, 1, &viewport);
    vkCmdSetScissor(command_buffer_[image_index], 0, 1, &scissor);

    render_game_objects(command_buffer_[image_index]);
    vkCmdEndRenderPass(command_buffer_[image_index]);
    if (vkEndCommandBuffer(command_buffer_[image_index]) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to record command buffer.");
    }
}

void FirstApp::render_game_objects(VkCommandBuffer command_buffer)
{
    pipeline_->bind(command_buffer);

    for (auto& obj : game_objects_)
    {
        obj.transform2d.rotation =
            glm::mod(obj.transform2d.rotation + 0.01f, glm::two_pi<float>());

        SimplePushConstantData push{};
        push.offset    = obj.transform2d.translation;
        push.color     = obj.color;
        push.transform = obj.transform2d.mat2();

        vkCmdPushConstants(command_buffer,
                           pipeline_layout_,
                           VK_SHADER_STAGE_VERTEX_BIT |
                               VK_SHADER_STAGE_FRAGMENT_BIT,
                           0,
                           sizeof(SimplePushConstantData),
                           &push);
        obj.model->bind(command_buffer);
        obj.model->draw(command_buffer);
    }
}

void FirstApp::draw_frame()
{
    uint32_t image_index;
    auto result = swap_chain_->acquireNextImage(&image_index);

    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        recreate_swap_chain();
        return;
    }

    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
    {
        throw std::runtime_error("Failed to acquire swap chain image!");
    }

    record_command_buffer(image_index);
    result = swap_chain_->submitCommandBuffers(&command_buffer_[image_index],
                                               &image_index);
    if (const auto v = {VK_ERROR_OUT_OF_DATE_KHR, VK_SUBOPTIMAL_KHR};
        std::ranges::any_of(v, [&result](auto val) { return result == val; }) ||
        window_.was_window_resized())
    {
        window_.reset_window_resized_flag();
        recreate_swap_chain();
        return;
    }

    if (result != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to present swap chain image!");
    }
}
} // namespace lve
