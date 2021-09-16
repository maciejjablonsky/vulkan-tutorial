#pragma once

#include <filesystem>
#include <rendering/device.hpp>
#include <vector>
#include <cstddef>

namespace lve
{
struct PipelineConfigInfo
{
    static PipelineConfigInfo create_default(uint32_t width, uint32_t height);
};

class LvePipeline
{
  public:
    LvePipeline(LveDevice& device,
                const std::filesystem::path& vertex_path,
                const std::filesystem::path& fragment_path,
                const PipelineConfigInfo& config);
    ~LvePipeline(){};
    
  private:
    void create_graphics_pipeline(const std::filesystem::path& vertex_path,
                                  const std::filesystem::path& fragment_path,
                                  const PipelineConfigInfo& config);

    void create_shader_module(const std::pmr::vector<std::byte>& code, VkShaderModule* shade_module);

    LveDevice& device_;
    VkPipeline graphics_pipeline_;
    VkShaderModule vert_shader_module_;
    VkShaderModule frag_shader_module_;
};
} // namespace lve
