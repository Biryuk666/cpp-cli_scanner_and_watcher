#pragma once

#include <cstdint>
#include <expected>
#include <filesystem>
#include <string>
#include <unordered_map>

#include "errors.h"

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

    std::expected<ScanResult, ScanError> scan_directory(
        const std::filesystem::path& root, const ScanOptions& option);
}  // namespace core