#include "scanner.h"

namespace core {
    static bool is_hidden(const std::filesystem::path& path) {
#ifdef _WIN32
        auto name = path.filename().wstring();
        return !name.empty() && name[0] == L'.';
#else
        auto name = path.filename().string();
        return !name.empty() && name[0] == '.';
#endif
    }

    static std::string extension_key(const std::filesystem::path& path) {
        return path.extension().string();
    }

    std::expected<ScanResult, ScanError> scan_directory(
        const std::filesystem::path& root, const ScanOptions& option) {
        ScanResult result;
        if (!std::filesystem::exists(root)) {
            ScanError error;
            error.code = ScanErrorCode::NotFound;
            error.message = "root does not exist";
            error.path1 = root;
            error.system_error_code =
                std::make_error_code(std::errc::no_such_file_or_directory);

            return std::unexpected(error);
        }
        if (!std::filesystem::is_directory(root)) {
            ScanError error;
            error.code = ScanErrorCode::NotDirectory;
            error.message = "root is not a directory";
            error.path1 = root;
            error.system_error_code =
                std::make_error_code(std::errc::not_a_directory);

            return std::unexpected(error);
        }

        for (const auto& entry :
             std::filesystem::recursive_directory_iterator(root)) {
            const auto& path = entry.path();

            if (!option.include_hidden && is_hidden(path)) {
                continue;
            }

            if (entry.is_directory()) {
                ++result.directory_count;
                continue;
            }

            if (!entry.is_regular_file()) {
                continue;
            }

            const auto size = entry.file_size();
            if (size < option.min_size_bytes) {
                continue;
            }

            result.file_count++;
            result.total_bytes += size;

            auto key = extension_key(path);
            auto& stats = result.by_extension[key];
            stats.count++;
            stats.bytes += size;
        }

        return result;
    }

}  // namespace core