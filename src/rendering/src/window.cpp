#include <rendering/window.hpp>
#include <stdexcept>

namespace lve
{
LveWindow::LveWindow(int w, int h, std::string name) : width_{w}, height_{h}, window_name_{name}
{
    init_window();
}

LveWindow::~LveWindow()
{
    glfwTerminate();
}

void LveWindow::init_window()
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    if (auto* ptr = glfwCreateWindow(width_, height_, window_name_.c_str(), nullptr, nullptr); ptr)
    {
        window_.reset(ptr);
    }
    else
    {
        throw std::runtime_error("Failed to create GLFWwindow.");
    }
}

void LveWindow::create_window_surface(VkInstance instance, VkSurfaceKHR* surface)
{

    if (glfwCreateWindowSurface(instance, window_.get(), nullptr, surface) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create window surface.");
    }
}
} // namespace lve
