#include <app/app.hpp>

namespace lve
{
void FirstApp::run()
{
    while (!window_.should_close())
    {
        glfwPollEvents();

    }
}
} // namespace lve
