find_package(fmt REQUIRED)

set(SOURCES include/app/app.hpp app.cpp)

add_library(app ${SOURCES})
add_library(lve::app ALIAS app)
target_link_libraries(app PUBLIC rendering)
target_include_directories(app PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(app_client main.cpp)
target_link_libraries(app_client PRIVATE lve::app fmt::fmt)

add_dependencies(app_client shaders)
