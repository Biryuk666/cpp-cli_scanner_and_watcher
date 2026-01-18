add_library(core STATIC
  scanner.cpp
  scanner.h
)

target_include_directories(core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_set_warnings(core)