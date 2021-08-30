from conans import ConanFile, CMake, tools
from conans.tools import vcvars
import os


class AtsFootersConan(ConanFile):
    name = "atsfooters"
    settings = "os", "compiler", "build_type", "arch"
    requires = (

    )
    generators = "cmake"

    options = {
    }

    default_options = {
    }

    version = None
    version_major = None
    version_minor = None
    version_patch = None
    with open(
            os.path.join(os.path.dirname(os.path.realpath(__file__)),
                         'version.txt')) as f:
        version = f.readline().rstrip()
        version_major = version.split('.')[0]
        version_minor = version.split('.')[1]
        version_patch = version.split('.')[2]

    def export(self):
        self.copy("version.txt")
        
    def export_sources(self):
        self.copy("*", excludes="build*")

    def build(self):
        cmake = CMake(self)
        cmake.configure(source_folder="atsfooters")
        cmake.build()
        cmake.test()

    def imports(self):
        self.copy("*.dll", dst="bin", src="bin")  # From bin to bin
        self.copy("*.dylib*", dst="bin", src="lib")  # From lib to bin
        self.copy("*.so*", dst="lib", keep_path=False)

    def package(self):
        self.copy("*.dll", dst="bin", keep_path=False)
        self.copy("*.lib", dst="lib", keep_path=False)
        self.copy("*", dst="include", src="atsfooters/include")
        self.copy("*.so*", dst="lib", keep_path=False)
        self.copy("*.a", dst="lib", keep_path=False)

    def package_info(self):
        self.cpp_info.includedirs = ["include"]
        self.cpp_info.libdirs = ["lib"]
        self.cpp_info.bindirs = ["bin"]
        self.cpp_info.libs = ["atsfooters"]

