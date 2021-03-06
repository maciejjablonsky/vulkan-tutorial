#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <array>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <tutorial/app.hpp>
#include <tutorial/camera.hpp>
#include <tutorial/simple_render_system.hpp>

namespace lve
{
FirstApp::FirstApp()
{
    load_game_objects();
}

void FirstApp::run()
{
    SimpleRenderSystem simple_render_system(
        device_, renderer_.get_swap_chain_render_pass());
    LveCamera camera{};
    // camera.set_view_direction(glm::vec3{0.f}, glm::vec3{0.5f, 0.f, 1.f});
    camera.set_view_target(glm::vec3{-1.f, -2.f, 2.f},
                           glm::vec3{0.f, 0.f, 2.5f});

    while (!window_.should_close())
    {
        glfwPollEvents();

        const auto aspect = renderer_.get_aspect_ratio();

        // camera.set_orthographic_projection(-aspect, aspect, -1, 1, -1, 1);
        camera.set_perspective_projection(
            glm::radians(50.f), aspect, 0.1f, 100.f);

        if (auto command_buffer = renderer_.begin_frame())
        {
            renderer_.begin_swap_chain_render_pass(command_buffer);
            simple_render_system.render_game_objects(
                command_buffer, game_objects_, camera);
            renderer_.end_swap_chain_render_pass(command_buffer);
            renderer_.end_frame();
        }
    }

    vkDeviceWaitIdle(device_.device());
}

std::unique_ptr<LveModel> create_cube_model(LveDevice& device, glm::vec3 offset)
{
    std::pmr::vector<LveModel::Vertex> vertices{

        // left face (white)
        {{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
        {{-.5f, .5f, .5f}, {.9f, .9f, .9f}},
        {{-.5f, -.5f, .5f}, {.9f, .9f, .9f}},
        {{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
        {{-.5f, .5f, -.5f}, {.9f, .9f, .9f}},
        {{-.5f, .5f, .5f}, {.9f, .9f, .9f}},

        // right face (yellow)
        {{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
        {{.5f, .5f, .5f}, {.8f, .8f, .1f}},
        {{.5f, -.5f, .5f}, {.8f, .8f, .1f}},
        {{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
        {{.5f, .5f, -.5f}, {.8f, .8f, .1f}},
        {{.5f, .5f, .5f}, {.8f, .8f, .1f}},

        // top face (orange, remember y axis points down)
        {{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
        {{.5f, -.5f, .5f}, {.9f, .6f, .1f}},
        {{-.5f, -.5f, .5f}, {.9f, .6f, .1f}},
        {{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
        {{.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
        {{.5f, -.5f, .5f}, {.9f, .6f, .1f}},

        // bottom face (red)
        {{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
        {{.5f, .5f, .5f}, {.8f, .1f, .1f}},
        {{-.5f, .5f, .5f}, {.8f, .1f, .1f}},
        {{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
        {{.5f, .5f, -.5f}, {.8f, .1f, .1f}},
        {{.5f, .5f, .5f}, {.8f, .1f, .1f}},

        // nose face (blue)
        {{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
        {{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
        {{-.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
        {{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
        {{.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
        {{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},

        // tail face (green)
        {{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
        {{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
        {{-.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
        {{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
        {{.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
        {{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},

    };
    for (auto& v : vertices)
    {
        v.position += offset;
    }
    return std::make_unique<LveModel>(device, vertices);
}

void FirstApp::load_game_objects()
{
    std::shared_ptr<LveModel> model =
        create_cube_model(device_, {.0f, .0f, .0f});
    auto cube                  = LveGameObject::create_game_object();
    cube.model                 = model;
    cube.transform.translation = {.0f, .0f, 2.5f};
    cube.transform.scale       = {.5f, .5f, .5f};
    game_objects_.push_back(std::move(cube));
}
} // namespace lve
