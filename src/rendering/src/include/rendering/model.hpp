#pragma once
#include <rendering/device.hpp>
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <vector>

namespace lve
{
class LveModel
{
  public:
    struct Vertex
    {
        glm::vec2 position;

        static std::pmr::vector<VkVertexInputBindingDescription>
        get_binding_description();
        static std::pmr::vector<VkVertexInputAttributeDescription>
        get_attribute_descriptions();
    };
    LveModel(LveDevice& device, const std::pmr::vector<Vertex>& vertices);
    ~LveModel();

    LveModel(const LveModel&) = delete;
    LveModel& operator=(const LveModel&) = delete;

    void bind(VkCommandBuffer command_buffer);
    void draw(VkCommandBuffer command_buffer);

  private:
    void create_vertex_buffers(const std::pmr::vector<Vertex>& vertices);
    LveDevice& device_;
    VkBuffer vertex_buffer_;
    VkDeviceMemory vertex_buffer_memory_;
    uint32_t vertex_count_;
};
} // namespace lve
