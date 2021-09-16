find_package(glfw3 REQUIRED)
find_package(Vulkan REQUIRED)
find_package(glm REQUIRED)

add_executable(vulkan-demo src/main.cpp)

target_link_libraries(vulkan-demo Vulkan::Vulkan glfw glm::glm)
