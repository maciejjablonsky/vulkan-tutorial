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
                         const PipelineConfigInfo& config)
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
    const PipelineConfigInfo& config)
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

    VkGraphicsPipelineCreateInfo pipeline_info{
        .sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .stageCount          = 2,
        .pStages             = shader_stages.data(),
        .pVertexInputState   = &vertex_input_info,
        .pInputAssemblyState = &config.input_assembly_info,
        .pViewportState      = &config.viewport_info,
        .pRasterizationState = &config.rasterization_info,
        .pMultisampleState   = &config.multisample_info,
        .pDepthStencilState  = &config.depth_stencil_info,
        .pColorBlendState    = &config.color_blend_info,
        .pDynamicState       = &config.dynamic_state_info,
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

void LvePipeline::default_pipeline_config_info(PipelineConfigInfo& config_info)
{
    config_info.input_assembly_info.sType =
        VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    config_info.input_assembly_info.topology =
        VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    config_info.input_assembly_info.primitiveRestartEnable = VK_FALSE;

    config_info.viewport_info.sType =
        VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    config_info.viewport_info.viewportCount = 1;
    config_info.viewport_info.pViewports    = nullptr;
    config_info.viewport_info.scissorCount  = 1;
    config_info.viewport_info.pScissors     = nullptr;

    config_info.rasterization_info.sType =
        VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    config_info.rasterization_info.depthClampEnable        = VK_FALSE;
    config_info.rasterization_info.rasterizerDiscardEnable = VK_FALSE;
    config_info.rasterization_info.polygonMode     = VK_POLYGON_MODE_FILL;
    config_info.rasterization_info.lineWidth       = 1.0f;
    config_info.rasterization_info.cullMode        = VK_CULL_MODE_NONE;
    config_info.rasterization_info.frontFace       = VK_FRONT_FACE_CLOCKWISE;
    config_info.rasterization_info.depthBiasEnable = VK_FALSE;
    config_info.rasterization_info.depthBiasConstantFactor = 0.0f; // Optional
    config_info.rasterization_info.depthBiasClamp          = 0.0f; // Optional
    config_info.rasterization_info.depthBiasSlopeFactor    = 0.0f; // Optional

    config_info.multisample_info.sType =
        VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    config_info.multisample_info.sampleShadingEnable   = VK_FALSE;
    config_info.multisample_info.rasterizationSamples  = VK_SAMPLE_COUNT_1_BIT;
    config_info.multisample_info.minSampleShading      = 1.0f;     // Optional
    config_info.multisample_info.pSampleMask           = nullptr;  // Optional
    config_info.multisample_info.alphaToCoverageEnable = VK_FALSE; // Optional
    config_info.multisample_info.alphaToOneEnable      = VK_FALSE; // Optional

    config_info.color_blend_attachment.colorWriteMask =
        VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
        VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    config_info.color_blend_attachment.blendEnable = VK_FALSE;
    config_info.color_blend_attachment.srcColorBlendFactor =
        VK_BLEND_FACTOR_ONE; // Optional
    config_info.color_blend_attachment.dstColorBlendFactor =
        VK_BLEND_FACTOR_ZERO; // Optional
    config_info.color_blend_attachment.colorBlendOp =
        VK_BLEND_OP_ADD; // Optional
    config_info.color_blend_attachment.srcAlphaBlendFactor =
        VK_BLEND_FACTOR_ONE; // Optional
    config_info.color_blend_attachment.dstAlphaBlendFactor =
        VK_BLEND_FACTOR_ZERO; // Optional
    config_info.color_blend_attachment.alphaBlendOp =
        VK_BLEND_OP_ADD; // Optional

    config_info.color_blend_info.sType =
        VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    config_info.color_blend_info.logicOpEnable   = VK_FALSE;
    config_info.color_blend_info.logicOp         = VK_LOGIC_OP_COPY; // Optional
    config_info.color_blend_info.attachmentCount = 1;
    config_info.color_blend_info.pAttachments =
        &config_info.color_blend_attachment;
    config_info.color_blend_info.blendConstants[0] = 0.0f; // Optional
    config_info.color_blend_info.blendConstants[1] = 0.0f; // Optional
    config_info.color_blend_info.blendConstants[2] = 0.0f; // Optional
    config_info.color_blend_info.blendConstants[3] = 0.0f; // Optional

    config_info.depth_stencil_info.sType =
        VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    config_info.depth_stencil_info.depthTestEnable       = VK_TRUE;
    config_info.depth_stencil_info.depthWriteEnable      = VK_TRUE;
    config_info.depth_stencil_info.depthCompareOp        = VK_COMPARE_OP_LESS;
    config_info.depth_stencil_info.depthBoundsTestEnable = VK_FALSE;
    config_info.depth_stencil_info.minDepthBounds        = 0.0f; // Optional
    config_info.depth_stencil_info.maxDepthBounds        = 1.0f; // Optional
    config_info.depth_stencil_info.stencilTestEnable     = VK_FALSE;
    config_info.depth_stencil_info.front                 = {}; // Optional
    config_info.depth_stencil_info.back                  = {}; // Optional

    config_info.dynamic_state_enables = {VK_DYNAMIC_STATE_VIEWPORT,
                                         VK_DYNAMIC_STATE_SCISSOR};
    config_info.dynamic_state_info.sType =
        VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    config_info.dynamic_state_info.pDynamicStates =
        config_info.dynamic_state_enables.data();
    config_info.dynamic_state_info.dynamicStateCount =
        static_cast<uint32_t>(config_info.dynamic_state_enables.size());
    config_info.dynamic_state_info.flags = 0;
}

} // namespace lve
