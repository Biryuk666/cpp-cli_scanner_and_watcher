if(BUILD_TESTING)
  include(GoogleTest)

  find_package(GTest CONFIG REQUIRED)

  add_executable(core_tests
    scanner_tests.cpp
  )
  add_executable(smoke_cli_tests smoke_cli_tests.cpp)
  add_test(NAME CLI.SmokeTests
    COMMAND smoke_cli_tests $<TARGET_FILE:app>
  )

  add_dependencies(smoke_cli_tests app)

  target_link_libraries(core_tests PRIVATE core GTest::gtest_main)

  target_set_warnings(core_tests)

  gtest_discover_tests(core_tests)
endif()