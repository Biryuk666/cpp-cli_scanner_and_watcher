#include "errors.h"

#include <magic_enum/magic_enum.hpp>

namespace core {
    std::string ScanError::to_string() {
        std::string result;
        if (code == ScanErrorCode::Unknown && message.empty()) return result;

        result += "ScanError { ";
        result += "code: " + std::string(magic_enum::enum_name(code)) + "; ";
        result += "message: " + message + "; ";
        result += path1.has_value()
                      ? ("path1: " + path1.value().string() + "; ")
                      : "";
        result += path2.has_value()
                      ? ("path1: " + path2.value().string() + "; ")
                      : "";
        result +=
            system_error_code
                ? "system_error_code: " + system_error_code.message() + "; "
                : "";
        result += " }";

        return result;
    }

    nlohmann::json ScanError::to_json() {
        nlohmann::json result;
        if (code == ScanErrorCode::Unknown && message.empty()) return result;

        // clang-format off
        result = {
            {"code", magic_enum::enum_name(code).data()},
            {"message", message}
        };
        // clang-format off

        if (path1.has_value()) {
            result["path1"] = path1.value().string();
        }

        if (path1.has_value()) {
            result["path2"] = path2.value().string();
        }

        if (system_error_code) {
            result["system_error_code"] = system_error_code.message();
        }

        return result;
    }
}  // namespace core