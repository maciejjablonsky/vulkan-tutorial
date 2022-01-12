find_package(glfw3 REQUIRED)
find_package(Vulkan REQUIRED)
find_package(glm REQUIRED)
find_package(fmt REQUIRED)

add_executable(udemy-demo main.cpp)

target_link_libraries(udemy-demo Vulkan::Vulkan glfw glm::glm fmt::fmt)