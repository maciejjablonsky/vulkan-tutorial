#pragma once

#include <cstddef>
#include <filesystem>
#include <rendering/device.hpp>
#include <vector>

namespace lve
{
struct PipelineConfigInfo
{
    VkViewport viewport;
    VkRect2D scissor;
    VkPipelineInputAssemblyStateCreateInfo input_assembly_info;
    VkPipelineRasterizationStateCreateInfo rasterization_info;
    VkPipelineMultisampleStateCreateInfo multisample_info;
    VkPipelineColorBlendAttachmentState color_blend_attachment;
    VkPipelineColorBlendStateCreateInfo color_blend_info;
    VkPipelineDepthStencilStateCreateInfo depth_stencil_info;
    VkPipelineLayout pipeline_layout = nullptr;
    VkRenderPass render_pass         = nullptr;
    uint32_t subpass                 = 0;
    static PipelineConfigInfo create_default(uint32_t width, uint32_t height);
};

class LvePipeline
{
  public:
    LvePipeline(
        LveDevice& device,
        const std::filesystem::path& vertex_path,
        const std::filesystem::path& fragment_path,
        PipelineConfigInfo
            config); // note: config must be passed by value, because
                     // otherwise passing by const & produces some optimization
                     // which loses config.color_blend_info.pAttachments
    ~LvePipeline();
    void bind(VkCommandBuffer command_buffer);

  private:
    void create_graphics_pipeline(const std::filesystem::path& vertex_path,
                                  const std::filesystem::path& fragment_path,
                                  PipelineConfigInfo config);

    void create_shader_module(const std::pmr::vector<std::byte>& code,
                              VkShaderModule* shade_module);

    LveDevice& device_;
    VkPipeline graphics_pipeline_;
    VkShaderModule vert_shader_module_;
    VkShaderModule frag_shader_module_;
};
} // namespace lve
