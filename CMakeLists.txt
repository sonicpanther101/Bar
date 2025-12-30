cmake_minimum_required(VERSION 3.15)

project(Bar)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add SableUI
find_package(GLEW REQUIRED)
add_subdirectory(vendor/SableUI)

add_definitions(-DGLEW_EGL)
add_definitions(-DGLEW_NO_GLU) # Often helps on Wayland/EGL

file(GLOB SOURCE "*.cpp" "*.h")
add_executable(Bar ${SOURCE})
target_link_libraries(Bar PRIVATE GLEW::GLEW)
target_link_libraries(Bar PRIVATE SableUI)