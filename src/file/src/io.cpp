#include <file/io.hpp>
#include <fmt/format.h>
#include <fstream>

namespace lve::file
{
std::pmr::vector<std::byte> load(const std::filesystem::path& file_path)
{
    std::basic_ifstream<std::byte> file(file_path, std::ios::binary);
    if (!file.is_open())
    {
        const auto path        = std::filesystem::absolute(file_path);
        const auto string_path = path.string();
        throw std::runtime_error(fmt::format("failed to open file: {}", string_path));
    }

    std::pmr::vector<std::byte> content_buffer(std::filesystem::file_size(file_path));
    file.read(content_buffer.data(), content_buffer.size());
    return content_buffer;
}
} // namespace lake::file