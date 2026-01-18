#include <spdlog/spdlog.h>

#include <CLI/CLI.hpp>
#include <filesystem>
#include <iostream>

#include "scanner.h"

int main(int argc, char** argv)
{
    CLI::App app{"CLI scanner"};

    std::string command;
    app.require_subcommand(1);

    core::ScanOptions option;
    std::filesystem::path path;

    auto* scan = app.add_subcommand("scan", "Scan directory recursively");
    scan->add_option("path", path, "Path to scan")->required();
    scan->add_flag("--include-hidden", option.include_hidden,
                   "Include hidden files");
    scan->add_option("--min-size", option.min_size_bytes,
                     "Minimum file size (bytes)")
        ->default_val(0);

    CLI11_PARSE(app, argc, argv);

    try
    {
        auto result = core::scan_directory(path, option);

        std::cout << "files: " << result.file_count << "\n";
        std::cout << "directories: " << result.directory_count << "\n";
        std::cout << "bytes: " << result.total_bytes << "\n";
        std::cout << "extensions:\n";
        for (const auto& [extension, stats] : result.by_extension)
        {
            std::cout << "  [" << (extension.empty() ? "<none>" : extension)
                      << "] "
                      << "count=" << stats.count << " bytes=" << stats.bytes
                      << "\n";
        }
        return 0;
    }
    catch (const std::filesystem::filesystem_error& error)
    {
        spdlog::error("filesystem error: {} (path1={}, path2={})", error.what(),
                      error.path1().string(), error.path2().string());
        return 2;
    }
    catch (const std::exception& exception)
    {
        spdlog::error("error: {}", exception.what());
        return 1;
    }
}
