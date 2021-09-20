#include <array>
#include <cassert>
#include <file/io.hpp>
#include <fmt/format.h>
#include <rendering/model.hpp>
#include <rendering/pipeline.hpp>

namespace lve
{
LvePipeline::LvePipeline(LveDevice& device,
                         const std::filesystem::path& vertex_path,
                         const std::filesystem::path& fragment_path,
                         PipelineConfigInfo config)
    : device_{device}
{
    create_graphics_pipeline(vertex_path, fragment_path, config);
}

LvePipeline::~LvePipeline()
{
    vkDestroyShaderModule(device_.device(), vert_shader_module_, nullptr);
    vkDestroyShaderModule(device_.device(), frag_shader_module_, nullptr);
    vkDestroyPipeline(device_.device(), graphics_pipeline_, nullptr);
}

void LvePipeline::bind(VkCommandBuffer command_buffer)
{
    vkCmdBindPipeline(
        command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphics_pipeline_);
}

void LvePipeline::create_graphics_pipeline(
    const std::filesystem::path& vertex_path,
    const std::filesystem::path& fragment_path,
    PipelineConfigInfo config)
{
    assert(config.pipeline_layout != VK_NULL_HANDLE &&
           "Cannot create graphics pipeline:: no pipelineLayout in config");
    assert(config.render_pass != VK_NULL_HANDLE &&
           "Cannot create graphics pipeline:: no renderPass in config");
    auto vert_code = file::load(vertex_path);
    auto frag_code = file::load(fragment_path);

    create_shader_module(vert_code, &vert_shader_module_);
    create_shader_module(frag_code, &frag_shader_module_);

    std::array<VkPipelineShaderStageCreateInfo, 2> shader_stages = {};
    shader_stages[0]                                             = {.sType =
                            VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                        .pNext               = nullptr,
                        .flags               = 0,
                        .stage               = VK_SHADER_STAGE_VERTEX_BIT,
                        .module              = vert_shader_module_,
                        .pName               = "main",
                        .pSpecializationInfo = nullptr};
    shader_stages[1]                                             = {.sType =
                            VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                        .pNext               = nullptr,
                        .flags               = 0,
                        .stage               = VK_SHADER_STAGE_FRAGMENT_BIT,
                        .module              = frag_shader_module_,
                        .pName               = "main",
                        .pSpecializationInfo = nullptr};

    const auto binding_descriptions =
        LveModel::Vertex::get_binding_description();
    const auto attribute_descriptions =
        LveModel::Vertex::get_attribute_descriptions();
    VkPipelineVertexInputStateCreateInfo vertex_input_info{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .vertexBindingDescriptionCount =
            static_cast<uint32_t>(binding_descriptions.size()),
        .pVertexBindingDescriptions = binding_descriptions.data(),
        .vertexAttributeDescriptionCount =
            static_cast<uint32_t>(attribute_descriptions.size()),
        .pVertexAttributeDescriptions = attribute_descriptions.data(),
    };

    VkPipelineViewportStateCreateInfo viewport_info = {
        .sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .viewportCount = 1,
        .pViewports    = &config.viewport,
        .scissorCount  = 1,
        .pScissors     = &config.scissor};

    VkGraphicsPipelineCreateInfo pipeline_info{
        .sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .stageCount          = 2,
        .pStages             = shader_stages.data(),
        .pVertexInputState   = &vertex_input_info,
        .pInputAssemblyState = &config.input_assembly_info,
        .pViewportState      = &viewport_info,
        .pRasterizationState = &config.rasterization_info,
        .pMultisampleState   = &config.multisample_info,
        .pDepthStencilState  = &config.depth_stencil_info,
        .pColorBlendState    = &config.color_blend_info,
        .pDynamicState       = nullptr,
        .layout              = config.pipeline_layout,
        .renderPass          = config.render_pass,
        .subpass             = config.subpass,
        .basePipelineHandle  = VK_NULL_HANDLE,
        .basePipelineIndex   = -1};
    if (vkCreateGraphicsPipelines(device_.device(),
                                  VK_NULL_HANDLE,
                                  1,
                                  &pipeline_info,
                                  nullptr,
                                  &graphics_pipeline_) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create graphics pipeline.");
    }
    fmt::print("vertex size: {}, fragment size: {}\n",
               vert_code.size(),
               frag_code.size());
}
void LvePipeline::create_shader_module(const std::pmr::vector<std::byte>& code,
                                       VkShaderModule* shader_module)
{
    VkShaderModuleCreateInfo create_info{
        .sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .codeSize = code.size(),
        .pCode    = reinterpret_cast<const uint32_t*>(code.data())};
    if (vkCreateShaderModule(
            device_.device(), &create_info, nullptr, shader_module) !=
        VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create shader module.");
    }
}

PipelineConfigInfo PipelineConfigInfo::create_default(uint32_t width,
                                                      uint32_t height)
{
    PipelineConfigInfo config{
        .viewport{.x        = 0.0f,
                  .y        = 0.0f,
                  .width    = static_cast<float>(width),
                  .height   = static_cast<float>(height),
                  .minDepth = 0.0f,
                  .maxDepth = 1.0f},
        .scissor{.offset{0, 0}, .extent{width, height}},
        .input_assembly_info{
            .sType =
                VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
            .topology               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
            .primitiveRestartEnable = VK_FALSE},
        .rasterization_info{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
            .depthClampEnable        = VK_FALSE,
            .rasterizerDiscardEnable = VK_FALSE,
            .polygonMode             = VK_POLYGON_MODE_FILL,
            .cullMode                = VK_CULL_MODE_NONE,
            .frontFace               = VK_FRONT_FACE_CLOCKWISE,
            .depthBiasEnable         = VK_FALSE,
            .depthBiasConstantFactor = 0.0f,
            .depthBiasClamp          = 0.0f,
            .depthBiasSlopeFactor    = 0.0f,
            .lineWidth               = 1.0f,
        },
        .multisample_info{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
            .rasterizationSamples  = VK_SAMPLE_COUNT_1_BIT,
            .sampleShadingEnable   = VK_FALSE,
            .minSampleShading      = 1.0f,
            .pSampleMask           = nullptr,
            .alphaToCoverageEnable = VK_FALSE,
            .alphaToOneEnable      = VK_FALSE},
        .color_blend_attachment{.blendEnable         = VK_FALSE,
                                .srcColorBlendFactor = VK_BLEND_FACTOR_ONE,
                                .dstColorBlendFactor = VK_BLEND_FACTOR_ZERO,
                                .colorBlendOp        = VK_BLEND_OP_ADD,
                                .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
                                .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
                                .alphaBlendOp        = VK_BLEND_OP_ADD,
                                .colorWriteMask = VK_COLOR_COMPONENT_R_BIT |
                                                  VK_COLOR_COMPONENT_G_BIT |
                                                  VK_COLOR_COMPONENT_B_BIT |
                                                  VK_COLOR_COMPONENT_A_BIT},
        .color_blend_info{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
            .logicOpEnable   = VK_FALSE,
            .logicOp         = VK_LOGIC_OP_COPY,
            .attachmentCount = 1,
            .blendConstants  = {0.0f, 0.0f, 0.0f, 0.0f}},
        .depth_stencil_info{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
            .depthTestEnable       = VK_TRUE,
            .depthWriteEnable      = VK_TRUE,
            .depthCompareOp        = VK_COMPARE_OP_LESS,
            .depthBoundsTestEnable = VK_FALSE,
            .stencilTestEnable     = VK_FALSE,
            .front                 = {},
            .back                  = {},
            .minDepthBounds        = 0.0f,
            .maxDepthBounds        = 1.0f}};

    config.color_blend_info.pAttachments = &config.color_blend_attachment;
    return config;
}

} // namespace lve
