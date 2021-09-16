#pragma once

#include <filesystem>

namespace lve
{
class LvePipeline
{
  public:
    LvePipeline(const std::filesystem::path& vertex_path, const std::filesystem::path& fragment_path);

  private:
    void create_graphics_pipeline(const std::filesystem::path& vertex_path,
                                  const std::filesystem::path& fragment_path);
};
} // namespace lve
