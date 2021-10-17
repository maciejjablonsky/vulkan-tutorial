#pragma once

#include <memory>
#include <rendering/device.hpp>
#include <rendering/game_object.hpp>
#include <rendering/model.hpp>
#include <rendering/renderer.hpp>
#include <rendering/window.hpp>
#include <vector>

namespace lve
{
class FirstApp
{
  public:
    static constexpr int WIDTH  = 800;
    static constexpr int HEIGHT = 600;
    void run();

    FirstApp();

  private:
    void load_game_objects();

    LveWindow window_{WIDTH, HEIGHT, "Hello Vulkan!"};
    LveDevice device_{window_};
    LveRenderer renderer_{window_, device_};
    std::pmr::vector<LveGameObject> game_objects_;
};
} // namespace lve

static_assert(!std::is_copy_constructible_v<lve::FirstApp>);
static_assert(!std::is_copy_assignable_v<lve::FirstApp>);
