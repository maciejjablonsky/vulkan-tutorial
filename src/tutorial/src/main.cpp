#include <cstdlib>
#include <fmt/format.h>
#include <stdexcept>
#include <tutorial/app.hpp>

int main()
{
    lve::FirstApp app{};
    try
    {
        app.run();
    }
    catch (const std::exception& e)
    {
        fmt::print(stderr, "{}\n", e.what());
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
