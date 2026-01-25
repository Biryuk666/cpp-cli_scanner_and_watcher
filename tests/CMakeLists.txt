if(BUILD_TESTING)
  include(GoogleTest)

  find_package(GTest CONFIG REQUIRED)

  add_executable(core_tests
    scanner_tests.cpp
  )
  target_link_libraries(core_tests PRIVATE core GTest::gtest_main)
  target_set_warnings(core_tests)
  gtest_discover_tests(core_tests)
endif()