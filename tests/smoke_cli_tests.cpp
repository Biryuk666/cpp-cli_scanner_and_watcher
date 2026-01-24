#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>

static std::string read_all(const std::filesystem::path& path)
{
    std::ifstream input(path, std::ios::binary);
    return std::string((std::istreambuf_iterator<char>(input)),
                       std::istreambuf_iterator<char>());
}

static int run_with_redirect(const std::string& exe, const std::string& args,
                             const std::filesystem::path& result_file,
                             const std::filesystem::path& error_file)
{
#ifdef _WIN32
    std::string cmd = "cmd.exe /c \"\"" + exe + "\" " + args + " >\"" +
                      result_file.string() + "\" 2>\"" + error_file.string() +
                      "\"\"";

#else
    std::string cmd = "sh -lc '\"" + exe + "\" " + args + " 1>\"" +
                      result_file.string() + "\" 2>\"" + error_file.string() +
                      "\"'";

#endif
    return std::system(cmd.c_str());
}

static void require(bool condition, const char* message)
{
    if (!condition)
    {
        throw std::runtime_error(message);
    }
}

int main(int argc, char** argv)
{
    if (argc < 2) return 2;

    std::string scanner = argv[1];

    auto root = std::filesystem::temp_directory_path() / "smoke_cli_tests";
    std::filesystem::remove_all(root);
    std::filesystem::create_directories(root);

    auto result_file = root / "result.json";
    auto error_file = root / "error.txt";

    // 1) success json
    int code =
        run_with_redirect(scanner, "scan . --json", result_file, error_file);
    auto output = read_all(result_file);
    require(code == 0, "scan . --json must return 0");
    require(output.find("\"by_extension\"") != std::string::npos,
            "must contain by_extension");

    std::cerr << "smoke_cli_test #1 - DONE" << std::endl;

    // 2) error json
    code = run_with_redirect(scanner, "scan __definetely_missing_path_ --json",
                             result_file, error_file);
    output = read_all(error_file);
    require(code != 0, "missing path must return non-zero");
    require(output.find("error") != std::string::npos,
            "must contain error in json");

    std::cerr << "smoke_cli_test #2 - DONE" << std::endl;

    std::filesystem::remove_all(root);
    return 0;
}