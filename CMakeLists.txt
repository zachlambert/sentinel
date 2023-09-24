cmake_minimum_required(VERSION 3.14)
project(sentinel)

# Setup

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
include(FetchContent)

set(BUILD_ADDITIONAL_TARGETS OFF)
if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(BUILD_ADDITIONAL_TARGETS ON)
endif()


# Dependencies

FetchContent_Declare(cpp-utils
    GIT_REPOSITORY https://github.com/zachlambert/cpp-utils.git
    GIT_TAG master
)
FetchContent_MakeAvailable(cpp-utils)


# Library

add_library(sentinel SHARED
  src/logger.cpp
  src/profiler.cpp
)
target_include_directories(sentinel PUBLIC include)
target_link_libraries(sentinel PUBLIC cpp-utils)

# Additional targets

if(BUILD_ADDITIONAL_TARGETS)
    add_executable(example_logger example/logger.cpp)
    target_link_libraries(example_logger sentinel)

    add_executable(example_profiler example/profiler.cpp)
    target_link_libraries(example_profiler sentinel)
endif()
