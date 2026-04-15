cmake_minimum_required(VERSION 3.16)
project(lotus-core VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories
include_directories(include)

# Source files
set(SOURCES
    src/utils.cpp
    src/rules_parser.cpp
    src/engine.cpp
    src/charset.cpp
    src/flattener.cpp
    src/spelling.cpp
    src/engine_utils.cpp
    src/engine.cpp
    src/rules_data.cpp
)

# Testing
enable_testing()

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/heads/main.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Library target
add_library(lotus-core SHARED ${SOURCES})
target_include_directories(lotus-core PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Example executable (MVP Test Runner)
add_executable(lotus-mvp-check src/main.cpp)
target_link_libraries(lotus-mvp-check lotus-core)

# Tests
add_executable(lotus-tests
    tests/test_utils.cpp
    tests/test_flattener.cpp
    tests/test_rules_parser.cpp
    tests/test_spelling.cpp
    tests/test_engine_utils.cpp
    tests/test_engine.cpp
    tests/test_parity.cpp
    tests/test_extended.cpp
)
target_link_libraries(lotus-tests lotus-core gtest_main)
add_test(NAME lotus-tests COMMAND lotus-tests)
