#pragma once
#include <cstdint>
#include <filesystem>
#include <string>
#include <unordered_map>

namespace core {
    struct ScanOptions {
        bool include_hidden = false;
        std::uintmax_t min_size_bytes = 0;
    };

    struct ExtensionStat {
        std::uint64_t count = 0;
        std::uintmax_t bytes = 0;
    };

    struct ScanResult {
        std::uint64_t file_count = 0;
        std::uint64_t directory_count = 0;
        std::uintmax_t total_bytes = 0;
        std::unordered_map<std::string, ExtensionStat> by_extension;
    };
    
    ScanResult scan_directory(const std::filesystem::path& root, const ScanOptions& option);
}