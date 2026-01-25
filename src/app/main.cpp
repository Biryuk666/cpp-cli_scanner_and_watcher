#include <spdlog/spdlog.h>

#include <CLI/CLI.hpp>
#include <filesystem>
#include <iostream>
#include <nlohmann/json.hpp>

#include "errors.h"
#include "scanner.h"

static std::ostream& print_error_word(std::ostream& output) {
    constexpr const char* RED = "\033[31m";
    constexpr const char* RESET = "\033[0m";

    output << "[" << RED << "error" << RESET << "]" << std::endl;

    return output;
}

int main(int argc, char** argv) {
    CLI::App app{"CLI scanner"};

    std::string command;
    app.require_subcommand(1);

    core::ScanOptions option{};
    std::filesystem::path path;
    nlohmann::json output;
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
            output["files"] = value.file_count;
            output["directories"] = value.directory_count;
            output["bytes"] = value.total_bytes;
            output["path"] = path.string();

            for (const auto& [extention, stats] : value.by_extension) {
                // clang-format off
                output["by_extension"][extention] = {
                    {"count", stats.count},
                    {"bytes", stats.bytes}
                };
                // clang-format on
            }

            std::cout << output.dump(2) << std::endl;
        } else {
            std::cout << "files: " << value.file_count << "\n";
            std::cout << "directories: " << value.directory_count << "\n";
            std::cout << "bytes: " << value.total_bytes << "\n";
            std::cout << "extensions:\n";
            for (const auto& [extension, stats] : value.by_extension) {
                std::cout << "  [" << (extension.empty() ? "<none>" : extension)
                          << "] "
                          << "count=" << stats.count << " bytes=" << stats.bytes
                          << "\n";
            }
        }

        return 0;
    } else {
        auto& error = result.error();

        if (json) {
            output = error.to_json();

            print_error_word(std::cerr);
            std::cerr << output.dump(2) << std::endl;
        } else {
            spdlog::error(error.to_string());

            print_error_word(std::cerr);
            std::cerr << output.dump(2);
        }

        return static_cast<int>(error.code);
    }
}
