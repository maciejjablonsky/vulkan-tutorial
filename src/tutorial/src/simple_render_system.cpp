#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <array>
#include <cmath>
#include <filesystem>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <tutorial/simple_render_system.hpp>


namespace lve
{
struct SimplePushConstantData
{
    glm::mat4 transform{1.f};
    alignas(16) glm::vec3 color;
};

SimpleRenderSystem::SimpleRenderSystem(LveDevice& device,
                                       VkRenderPass render_pass)
    : device_(device)
{
    create_pipeline_layout();
    create_pipeline(render_pass);
}

SimpleRenderSystem::~SimpleRenderSystem()
{
    vkDestroyPipelineLayout(device_.device(), pipeline_layout_, nullptr);
}

void SimpleRenderSystem::create_pipeline_layout()
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

void SimpleRenderSystem::create_pipeline(VkRenderPass render_pass)
{
    assert(pipeline_layout_ != nullptr &&
           "Cannot create pipeline before pipeline layout");
    PipelineConfigInfo pipeline_config{};
    LvePipeline::default_pipeline_config_info(pipeline_config);
    pipeline_config.render_pass     = render_pass;
    pipeline_config.pipeline_layout = pipeline_layout_;
    auto shaders_path               = std::filesystem::path{SHADERS_DIRECTORY};
    pipeline_ =
        std::make_unique<LvePipeline>(device_,
                                      shaders_path / "simple_shader.vert.spv",
                                      shaders_path / "simple_shader.frag.spv",
                                      pipeline_config);
}

void SimpleRenderSystem::render_game_objects(
    VkCommandBuffer command_buffer,
    std::pmr::vector<LveGameObject>& game_objects,
    const LveCamera& camera)
{
    pipeline_->bind(command_buffer);

    const auto projection_view = camera.get_projection() * camera.get_view();

    for (auto& obj : game_objects)
    {
        obj.transform.rotation =
            glm::mod(obj.transform.rotation + glm::vec3{0.01f, 0.02f, 0.02f},
                     glm::two_pi<float>());

        SimplePushConstantData push{};
        push.color     = obj.color;
        push.transform = projection_view * obj.transform.mat4();

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
} // namespace lve