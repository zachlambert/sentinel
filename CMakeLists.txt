cmake_minimum_required(VERSION 3.14)
project(sentinel)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# cpp-utils
FetchContent_Declare(cpp-utils
    GIT_REPOSITORY https://github.com/zachlambert/cpp-utils.git
    GIT_TAG master
)
FetchContent_MakeAvailable(cpp-utils)

add_library(sentinel SHARED
  src/logger.cpp
  src/profiler.cpp
)
target_include_directories(sentinel PUBLIC include)
target_link_libraries(sentinel PUBLIC cpp-utils)

add_executable(example_logger example/logger.cpp)
target_link_libraries(example_logger sentinel)

add_executable(example_profiler example/profiler.cpp)
target_link_libraries(example_profiler sentinel)
