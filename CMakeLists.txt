cmake_minimum_required(VERSION 3.15)
project(vulkan-tutorial)

if (EXISTS ${CMAKE_BINARY_DIR}/conan_paths.cmake)
    include(${CMAKE_BINARY_DIR}/conan_paths.cmake)
endif()

add_subdirectory(src)
