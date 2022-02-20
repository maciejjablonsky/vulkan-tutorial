find_package(fmt REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)
find_package(Vulkan REQUIRED)

add_executable(app 
    app.cpp
    camera.cpp
    device.cpp
    main.cpp
    model.cpp
    pipeline.cpp
    renderer.cpp
    simple_render_system.cpp
    swap_chain.cpp
    window.cpp
 )

target_link_libraries(app PRIVATE fmt::fmt glfw Vulkan::Vulkan lve::file glm::glm)
target_compile_features(app PRIVATE cxx_std_20)

target_include_directories(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(app PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_dependencies(app shaders)