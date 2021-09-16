#pragma once

#include <cstddef>
#include <filesystem>
#include <vector>

namespace lve::file
{
std::pmr::vector<std::byte> load(const std::filesystem::path& file_path);
}