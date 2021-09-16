#pragma once

#include <rendering/window.hpp>

namespace lve
{
class FirstApp
{
  public:
    static constexpr int WIDTH  = 800;
    static constexpr int HEIGHT = 600;
    void run();
  private:
    LveWindow window_{WIDTH, HEIGHT, "Hello Vulkan!"};
};
} // namespace lve
