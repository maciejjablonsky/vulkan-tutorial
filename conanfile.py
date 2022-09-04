from conans import ConanFile, CMake
from conan.tools.cmake import CMake, CMakeDeps, CMakeToolchain

class VulkanTutorial(ConanFile):
    version = '1.0.0'
    name = 'VulkanTutorial'
    requires = 'glfw/3.3.4', 'glm/0.9.9.8', 'fmt/8.0.1', 'vulkan-loader/1.3.224.0'
    build_requires = 'shaderc/2021.1'
    generators = 'CMakeToolchain', 'CMakeDeps'
    settings = 'os', 'compiler', 'arch', 'build_type'

    def generate(self):
        tc = CMakeToolchain(self)
        tc.generate()

        deps = CMakeDeps(self)
        deps.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
