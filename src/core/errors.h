#pragma once

#include <filesystem>
#include <nlohmann/json.hpp>
#include <optional>
#include <string>
#include <system_error>

namespace core {
    enum class ScanErrorCode {
        Unknown = 0,
        NotFound = 1,
        NotDirectory = 2,
        PermissionDenied = 3,
        IOError = 4,
        InvalidArgument = 5
    };

    struct ScanError {
        ScanErrorCode code;
        std::string message;
        std::optional<std::filesystem::path> path1, path2;
        std::error_code system_error_code;

        std::string to_string();
        nlohmann::json to_json();
    };
}  // namespace core