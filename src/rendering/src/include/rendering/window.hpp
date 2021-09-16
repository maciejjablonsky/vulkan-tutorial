#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <memory>
#include <string>

namespace lve

{
class LveWindow
{
  public:
    LveWindow(int w, int h, std::string name);
    ~LveWindow();

    bool should_close()
    {
        return glfwWindowShouldClose(window_.get());
    }

  private:
    void init_window();

    const int width_;
    const int height_;
    std::string window_name_;
    
    struct glfw_window_deleter
    {
        void operator()(GLFWwindow* window)
        {
            glfwDestroyWindow(window);
        };
    };
    std::unique_ptr<GLFWwindow, glfw_window_deleter> window_;
};
} // namespace lve

static_assert(!std::is_copy_constructible_v<lve::LveWindow>);
