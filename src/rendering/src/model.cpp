#include <array>
#include <cassert>
#include <rendering/model.hpp>

namespace lve
{
LveModel::LveModel(LveDevice& device, const std::pmr::vector<Vertex>& vertices)
    : device_{device}
{
    create_vertex_buffers(vertices);
}

LveModel::~LveModel()
{
    vkDestroyBuffer(device_.device(), vertex_buffer_, nullptr);
    vkFreeMemory(device_.device(), vertex_buffer_memory_, nullptr);
}

void LveModel::create_vertex_buffers(const std::pmr::vector<Vertex>& vertices)
{
    vertex_count_ = static_cast<uint32_t>(vertices.size());
    assert(vertex_count_ >= 3 && "Vertex count must be at least 3");
    VkDeviceSize buffer_size = sizeof(vertices.front()) * vertex_count_;
    device_.createBuffer(buffer_size,
                         VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                         VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                             VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                         vertex_buffer_,
                         vertex_buffer_memory_);
    void* data = nullptr;
    vkMapMemory(
        device_.device(), vertex_buffer_memory_, 0, buffer_size, 0, &data);
    std::memcpy(data, vertices.data(), static_cast<size_t>(buffer_size));
    vkUnmapMemory(device_.device(), vertex_buffer_memory_);
}

void LveModel::bind(VkCommandBuffer command_buffer)
{
    std::array<VkBuffer, 1> buffers     = {vertex_buffer_};
    std::array<VkDeviceSize, 1> offsets = {0};
    vkCmdBindVertexBuffers(
        command_buffer, 0, 1, buffers.data(), offsets.data());
}

void LveModel::draw(VkCommandBuffer command_buffer)
{
    vkCmdDraw(command_buffer, vertex_count_, 1, 0, 0);
}

std::pmr::vector<VkVertexInputBindingDescription> LveModel::Vertex::
    get_binding_description()
{
    std::pmr::vector<VkVertexInputBindingDescription> binding_descriptions{1};
    binding_descriptions[0].binding   = 0;
    binding_descriptions[0].stride    = sizeof(Vertex);
    binding_descriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    return binding_descriptions;
}

std::pmr::vector<VkVertexInputAttributeDescription> LveModel::Vertex::
    get_attribute_descriptions()
{
    std::pmr::vector<VkVertexInputAttributeDescription> attribute_descriptions{
        1};
    attribute_descriptions[0].binding  = 0;
    attribute_descriptions[0].location = 0;
    attribute_descriptions[0].format   = VK_FORMAT_R32G32_SFLOAT;
    attribute_descriptions[0].offset   = 0;
    return attribute_descriptions;
}
} // namespace lve
