find_package(Vulkan REQUIRED COMPONENTS glslc)
find_program(glslc_executable NAMES glslc HINTS Vulkan::glslc)

file(
    GLOB
    SHADERS_FILES
    *.vert
    *.frag)

message(STATUS "Shaders: ${SHADERS_FILES}")

add_custom_command(
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/shaders
    OUTPUT ${CMAKE_BINARY_DIR}/shaders
    COMMENT "Creating ${CMAKE_BINARY_DIR}/shaders")

foreach(SOURCE IN LISTS SHADERS_FILES)
    get_filename_component(
        FILENAME
        ${SOURCE}
        NAME)
    add_custom_command(
        COMMAND ${glslc_executable} -o
                ${CMAKE_BINARY_DIR}/shaders/${FILENAME}.spv ${SOURCE}
        OUTPUT ${CMAKE_BINARY_DIR}/shaders/${FILENAME}.spv
        DEPENDS ${SOURCE} ${CMAKE_BINARY_DIR}/shaders
        COMMENT "Compiling ${FILENAME}")
    list(
        APPEND
        SPV_SHADERS
        ${CMAKE_BINARY_DIR}/shaders/${FILENAME}.spv)
endforeach()

add_custom_target(shaders ALL DEPENDS ${SPV_SHADERS})
