from conans import ConanFile, CMake

class LeetCodeConan(ConanFile):
    name = "Boost_Python_Demo"
    author = "Andrew Hayman"
    settings = "os", "compiler", "build_type", "arch"
    generators = "cmake_find_package"

    # These options ensure we are building Boost to enable Boost.Python
    def configure(self):
        self.options["boost"].without_python = False
        self.options["boost"].python_executable = "python.exe"
        self.options["boost"].filesystem_no_deprecated = True

    # Define Boost as a requirement. More packages can be added here as needed. 
    # This is completed when 'conan install' is used.
    def build_requirements(self):
        self.build_requires("boost/1.79.0")

    # Build command when 'conan build' is used. Calls CMake.
    def build(self):
        cmake = CMake(self)
        cmake.configure(source_folder=self.source_folder, build_folder=self.build_folder)
        cmake.build()