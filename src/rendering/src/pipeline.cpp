#include <file/io.hpp>
#include <fmt/format.h>
#include <rendering/pipeline.hpp>

namespace lve
{
LvePipeline::LvePipeline(const std::filesystem::path& vertex_path, const std::filesystem::path& fragment_path)
{
    create_graphics_pipeline(vertex_path, fragment_path);
}
void LvePipeline::create_graphics_pipeline(const std::filesystem::path& vertex_path,
                                           const std::filesystem::path& fragment_path)
{
    auto vert_code = file::load(vertex_path);
    auto frag_code = file::load(fragment_path);

    fmt::print("vertex size: {}, fragment size: {}\n", vert_code.size(), frag_code.size());
}
} // namespace lve
