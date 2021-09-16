#include <file/io.hpp>
#include <fmt/format.h>
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
void LvePipeline::create_graphics_pipeline(const std::filesystem::path& vertex_path,
                                           const std::filesystem::path& fragment_path,
                                           const PipelineConfigInfo& config)
{
    auto vert_code = file::load(vertex_path);
    auto frag_code = file::load(fragment_path);

    fmt::print("vertex size: {}, fragment size: {}\n", vert_code.size(), frag_code.size());
}
void LvePipeline::create_shader_module(const std::pmr::vector<std::byte>& code, VkShaderModule* shader_module)
{
    VkShaderModuleCreateInfo create_info{.sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
                                         .codeSize = code.size(),
                                         .pCode    = reinterpret_cast<const uint32_t*>(code.data())};
    if (vkCreateShaderModule(device_.device(), &create_info, nullptr, shader_module) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create shader module.");
    }
}

PipelineConfigInfo PipelineConfigInfo::create_default(uint32_t width , uint32_t height)
{
    PipelineConfigInfo config_info{};
    return config_info;
}

} // namespace lve
