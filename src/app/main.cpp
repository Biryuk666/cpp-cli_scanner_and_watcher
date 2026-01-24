#include <spdlog/spdlog.h>

#include <CLI/CLI.hpp>
#include <filesystem>
#include <iostream>
#include <nlohmann/json.hpp>

#include "scanner.h"
static std::ostream& print_error_word(std::ostream& output)
{
    constexpr const char* RED = "\033[31m";
    constexpr const char* RESET = "\033[0m";

    output << "[" << RED << "error" << RESET << "]" << std::endl;

    return output;
}

int main(int argc, char** argv)
{
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

    try
    {
        auto result = core::scan_directory(path, option);

        if (json)
        {
            output["files"] = result.file_count;
            output["directories"] = result.directory_count;
            output["bytes"] = result.total_bytes;
            output["path"] = path.string();

            for (const auto& [extention, stats] : result.by_extension)
            {
                // clang-format off
                output["by_extension"][extention] =  {
                    {"count", stats.count},
                    {"bytes", stats.bytes}
                };
                // clang-format off
            }

            std::cout << output.dump(2) << std::endl;
        }
        else
        {
            std::cout << "files: " << result.file_count << "\n";
            std::cout << "directories: " << result.directory_count << "\n";
            std::cout << "bytes: " << result.total_bytes << "\n";
            std::cout << "extensions:\n";
            for (const auto& [extension, stats] : result.by_extension)
            {
                std::cout << "  [" << (extension.empty() ? "<none>" : extension) << "] "
                          << "count=" << stats.count << " bytes=" << stats.bytes << "\n";
            }
        }

        return 0;
    }
    catch (const std::filesystem::filesystem_error& error)
    {
        if (json)
        {
            
            // clang-format off
            output = {
                {"type", "filesystem error"},
                {"message", error.what()},
                {"paths", {
                    {"path1", error.path1().string()},
                    {"path2", error.path2().string()}
                }}
            };
            // clang-format off

            print_error_word(std::cerr);
            std::cerr << output.dump(2) << std::endl;
        }
        else
        {
            spdlog::error("filesystem error: {} (path1={}, path2={})",
                          error.what(),
                          error.path1().string(),
                          error.path2().string());
        }
        return 2;
    }
    catch (const std::exception& exception)
    {
        if (json)
        {
            output["error"] = exception.what();

            print_error_word(std::cerr);
            std::cerr << output.dump(2);
        }
        else
        {
            spdlog::error("error: {}", exception.what());
        }

        return 1;
    }
}
