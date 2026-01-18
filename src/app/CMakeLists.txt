find_package(CLI11 CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

add_executable(app main.cpp)
target_link_libraries(app PRIVATE
    core
    CLI11::CLI11
    spdlog::spdlog
    fmt::fmt
    nlohmann_json::nlohmann_json)
target_set_warnings(app)