from conans import ConanFile, CMake

class VulkanTutorial(ConanFile):
    version = '1.0.0'
    name = 'VulkanTutorial'
    requires = 'glfw/3.3.4', 'glm/0.9.9.8', 'fmt/8.0.1'
    generators = 'cmake_find_package', 'cmake_paths'
    settings = 'os', 'compiler', 'arch', 'build_type'
    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
