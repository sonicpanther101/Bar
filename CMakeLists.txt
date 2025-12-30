cmake_minimum_required(VERSION 3.15)

project(Bar)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add SableUI
add_subdirectory(vendor/SableUI)

file(GLOB_RECURSE SOURCE "*.cpp" "*.h")
add_executable(Bar ${SOURCE})
target_link_libraries(Bar PRIVATE SableUI)