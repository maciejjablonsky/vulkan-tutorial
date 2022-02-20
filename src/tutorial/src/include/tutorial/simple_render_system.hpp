#pragma once

#include <memory>
#include <tutorial/camera.hpp>
#include <tutorial/device.hpp>
#include <tutorial/game_object.hpp>
#include <tutorial/model.hpp>
#include <tutorial/pipeline.hpp>
#include <vector>
namespace lve
{
class SimpleRenderSystem
{
  public:
    SimpleRenderSystem(LveDevice& device, VkRenderPass render_pass);
    ~SimpleRenderSystem();

    void render_game_objects(VkCommandBuffer command_buffer,
                             std::pmr::vector<LveGameObject>& game_objects,
                             const LveCamera& camera);

  private:
    void create_pipeline_layout();
    void create_pipeline(VkRenderPass render_pass);

    LveDevice& device_;
    std::unique_ptr<LvePipeline> pipeline_;
    VkPipelineLayout pipeline_layout_;
};
} // namespace lve

static_assert(!std::is_copy_constructible_v<lve::SimpleRenderSystem>);
static_assert(!std::is_copy_assignable_v<lve::SimpleRenderSystem>);
