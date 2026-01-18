#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <string>

#include "scanner.h"

using namespace std::filesystem;

static void write_bytes(const path& path, std::size_t file_size)
{
    std::ofstream out(path, std::ios::binary);
    ASSERT_TRUE(out.good());
    std::string buffer(file_size, 'x');
    out.write(buffer.data(), static_cast<std::streamsize>(buffer.size()));
    out.close();
}

TEST(Scanner, CountsFilesDirsAndBytes)
{
    path root = temp_directory_path() / path("scanner_test_root");
    remove_all(root);
    create_directories(root / "a");
    create_directories(root / "b" / "c");

    write_bytes(root / "one.txt", 10);
    write_bytes(root / "a" / "two.bin", 7);
    write_bytes(root / "b" / "c" / "three", 5);

    core::ScanOptions option;
    option.include_hidden = true;
    option.min_size_bytes = 0;

    auto result = core::scan_directory(root, option);

    remove_all(root);

    EXPECT_EQ(result.file_count, 3u);
    EXPECT_EQ(result.directory_count, 3u);
    EXPECT_EQ(result.total_bytes, 22u);

    ASSERT_TRUE(result.by_extension.contains(".txt"));
    ASSERT_TRUE(result.by_extension.contains(".bin"));
    ASSERT_TRUE(result.by_extension.contains(""));

    EXPECT_EQ(result.by_extension[".txt"].count, 1u);
    EXPECT_EQ(result.by_extension[".txt"].bytes, 10u);

    EXPECT_EQ(result.by_extension[".bin"].count, 1u);
    EXPECT_EQ(result.by_extension[".bin"].bytes, 7u);

    EXPECT_EQ(result.by_extension[""].count, 1u);
    EXPECT_EQ(result.by_extension[""].bytes, 5u);
}

TEST(Scanner, RespectsMinSize)
{
    path root = temp_directory_path() / path("scanner_test_minsize");
    remove_all(root);
    create_directories(root);

    write_bytes(root / "small.txt", 3);
    write_bytes(root / "big.txt", 10);

    core::ScanOptions option;
    option.include_hidden = true;
    option.min_size_bytes = 5;

    auto result = core::scan_directory(root, option);

    remove_all(root);

    EXPECT_EQ(result.file_count, 1u);
    EXPECT_EQ(result.total_bytes, 10u);
    EXPECT_EQ(result.by_extension[".txt"].count, 1u);
    EXPECT_EQ(result.by_extension[".txt"].bytes, 10u);
}

TEST(Scanner, ThrowsIfRootMissing)
{
    core::ScanOptions option;
    path missing = temp_directory_path() / path("scanner_test_missing_12345");
    remove_all(missing);

    EXPECT_THROW((void)core::scan_directory(missing, option), filesystem_error);
}
