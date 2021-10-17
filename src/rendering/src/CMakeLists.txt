find_package(glfw3 REQUIRED)
find_package(Vulkan REQUIRED)
find_package(glm REQUIRED)
find_package(fmt REQUIRED)

set(SOURCES
    include/rendering/window.hpp
    window.cpp
    include/rendering/pipeline.hpp
    pipeline.cpp
    include/rendering/device.hpp
    device.cpp
    include/rendering/swap_chain.hpp
    swap_chain.cpp
    include/rendering/model.hpp
    model.cpp
    include/rendering/game_object.hpp
    include/rendering/renderer.hpp
    renderer.cpp
    include/rendering/simple_render_system.hpp
    simple_render_system.cpp)

add_library(rendering ${SOURCES})
add_library(lve::rendering ALIAS rendering)
target_link_libraries(rendering PUBLIC glfw Vulkan::Vulkan lve::file fmt::fmt
                                       glm::glm)
target_include_directories(rendering PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_features(rendering PUBLIC cxx_std_20)
