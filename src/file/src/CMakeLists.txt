find_package(fmt REQUIRED)

set(SOURCES
    include/file/io.hpp
    io.cpp
)

add_library(file ${SOURCES})
target_include_directories(file PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(file PRIVATE fmt::fmt)
target_compile_features(file PUBLIC cxx_std_17)
