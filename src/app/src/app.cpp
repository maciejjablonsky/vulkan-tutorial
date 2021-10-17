#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <app/app.hpp>
#include <array>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <rendering/simple_render_system.hpp>

namespace lve
{
FirstApp::FirstApp()
{
    load_game_objects();
}

FirstApp::~FirstApp()
{
}

void FirstApp::run()
{
    SimpleRenderSystem simple_render_system(
        device_, renderer_.get_swap_chain_render_pass());
    while (!window_.should_close())
    {
        glfwPollEvents();
        if (auto command_buffer = renderer_.begin_frame())
        {
            renderer_.begin_swap_chain_render_pass(command_buffer);
            simple_render_system.render_game_objects(command_buffer,
                                                     game_objects_);
            renderer_.end_swap_chain_render_pass(command_buffer);
            renderer_.end_frame();
        }
    }

    vkDeviceWaitIdle(device_.device());
}

void FirstApp::load_game_objects()
{
    std::pmr::vector<LveModel::Vertex> vertices{
        {{0.0f, -0.5f}, {1.f, 0.f, 0.f}},
        {{0.5f, 0.5f}, {0.f, 1.f, 0.f}},
        {{-0.5f, 0.5f}, {0.f, 0.f, 1.f}}};
    constexpr auto depth = 5;
    // std::pmr::vector<LveModel::Vertex> vertices(3 * (std::powl(3, depth)));
    // sierpinski(vertices, depth, {-1.f, 1.f}, {1.f, 1.f}, {0.0f, -1.f});
    auto model     = std::make_shared<LveModel>(device_, vertices);
    auto triangle  = LveGameObject::create_game_object();
    triangle.model = model;
    triangle.color = {.1f, .8f, .1f};
    triangle.transform2d.translation.x = .2f;
    triangle.transform2d.scale         = {2.f, .5f};
    triangle.transform2d.rotation      = .25f * glm::two_pi<float>();
    game_objects_.push_back(std::move(triangle));
}
} // namespace lve
