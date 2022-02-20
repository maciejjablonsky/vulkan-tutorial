#pragma once

#include <cstddef>
#include <filesystem>
#include <tutorial/device.hpp>
#include <vector>

namespace lve
{
struct PipelineConfigInfo
{
    PipelineConfigInfo()                          = default;
    PipelineConfigInfo(const PipelineConfigInfo&) = delete;
    PipelineConfigInfo& operator=(const PipelineConfigInfo&) = delete;

    VkPipelineViewportStateCreateInfo viewport_info;
    VkPipelineInputAssemblyStateCreateInfo input_assembly_info;
    VkPipelineRasterizationStateCreateInfo rasterization_info;
    VkPipelineMultisampleStateCreateInfo multisample_info;
    VkPipelineColorBlendAttachmentState color_blend_attachment;
    VkPipelineColorBlendStateCreateInfo color_blend_info;
    VkPipelineDepthStencilStateCreateInfo depth_stencil_info;
    std::vector<VkDynamicState> dynamic_state_enables;
    VkPipelineDynamicStateCreateInfo dynamic_state_info;
    VkPipelineLayout pipeline_layout = nullptr;
    VkRenderPass render_pass         = nullptr;
    uint32_t subpass                 = 0;
};

class LvePipeline
{
  public:
    LvePipeline(LveDevice& device,
                const std::filesystem::path& vertex_path,
                const std::filesystem::path& fragment_path,
                const PipelineConfigInfo& config);
    ~LvePipeline();
    void bind(VkCommandBuffer command_buffer);
    static void default_pipeline_config_info(PipelineConfigInfo& config_info);

  private:
    void create_graphics_pipeline(const std::filesystem::path& vertex_path,
                                  const std::filesystem::path& fragment_path,
                                  const PipelineConfigInfo& config);

    void create_shader_module(const std::pmr::vector<std::byte>& code,
                              VkShaderModule* shade_module);

    LveDevice& device_;
    VkPipeline graphics_pipeline_;
    VkShaderModule vert_shader_module_;
    VkShaderModule frag_shader_module_;
};
} // namespace lve
