#include <spdlog/spdlog.h>

#include <CLI/CLI.hpp>
#include <filesystem>
#include <fmt/core.h>
#include <fmt/format.h>
#include <iostream>
#include <nlohmann/json.hpp>
#include <fmt/printf.h>

#include "errors.h"
#include "scanner.h"

static std::ostream& print_error_word(std::ostream& output) {
    constexpr const char* RED = "\033[31m";
    constexpr const char* RESET = "\033[0m";

    fmt::print("[{}error{}]\n", RED, RESET);

    return output;
}

int main(int argc, char** argv) {
    CLI::App app{"CLI scanner"};

    std::string command;
    app.require_subcommand(1);

    core::ScanOptions option{};
    std::filesystem::path path;
    bool json = false;

    auto* scan = app.add_subcommand("scan", "Scan directory recursively");
    scan->add_option("path", path, "Path to scan")->required();
    scan->add_flag("--json", json);
    scan->add_flag("--include-hidden", option.include_hidden,
                   "Include hidden files");
    scan->add_option("--min-size", option.min_size_bytes,
                     "Minimum file size (bytes)")
        ->default_val(0);

    CLI11_PARSE(app, argc, argv);
    auto result = core::scan_directory(path, option);
    if (result.has_value()) {
        auto& value = result.value();

        if (json) {
            auto output = value.to_json();

            std::cout << output.dump(2) << std::endl;
        } else {
            auto output = value.to_string();
            fmt::print("{}", output);
        }

        return 0;
    } else {
        auto& error = result.error();

        if (json) {
            auto output = error.to_json();

            print_error_word(std::cerr);
            std::cerr << output.dump(2) << std::endl;
        } else {
            spdlog::error(error.to_string());
        }

        return static_cast<int>(error.code);
    }
}
