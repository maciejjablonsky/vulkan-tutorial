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

    void create_window_surface(VkInstance instance, VkSurfaceKHR* surface);
    VkExtent2D get_extent()
    {
        return {static_cast<uint32_t>(width_), static_cast<uint32_t>(height_)};
    }
    bool was_window_resized() const
    {
        return framebuffer_resized_;
    }

    void reset_window_resized_flag()
    {
        framebuffer_resized_ = false;
    }

  private:
    void init_window();
    static void framebuffer_resized_callback(GLFWwindow* window,
                                             int width,
                                             int height);
    int width_;
    int height_;
    std::string window_name_;
    bool framebuffer_resized_ = false;

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
