from conan import ConanFile
from conan.tools.cmake import cmake_layout

class CliScannerConan(ConanFile):
    name = "cli-scanner-and-watcher"
    version = "0.1.0"
    settings = "os", "arch", "compiler", "build_type"

    requires = (
        "gtest/1.17.0",
        "fmt/12.1.0",
        "spdlog/1.17.0",
        "cli11/2.6.0",
        "nlohmann_json/3.12.0",
        "magic_enum/0.9.7"
    )

    generators = ("CMakeDeps", "CMakeToolchain")

    def layout(self):
        cmake_layout(self)