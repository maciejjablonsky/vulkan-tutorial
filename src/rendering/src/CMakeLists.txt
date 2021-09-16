find_package(glfw3 REQUIRED)
find_package(Vulkan REQUIRED)
find_package(glm REQUIRED)

set(SOURCES
    include/rendering/window.hpp
    window.cpp
)

add_library(rendering ${SOURCES})
target_link_libraries(rendering PUBLIC glfw Vulkan::Vulkan)
target_include_directories(rendering PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_features(rendering PUBLIC cxx_std_17)
