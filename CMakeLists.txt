cmake_minimum_required(VERSION 3.15)
project(Bar)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(PkgConfig REQUIRED)

# Only need basic OpenGL/GLFW dependencies for SableUI
pkg_check_modules(GLFW3 REQUIRED glfw3)

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)

# Add SableUI subdirectory
add_subdirectory(vendor/SableUI)

file(GLOB SOURCES "src/*.cpp" "src/*.h")

# Just your main.cpp
add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/SableUI/include
    ${GLFW3_INCLUDE_DIRS}
    ${OPENGL_INCLUDE_DIR}
)

# Link against SableUI and its dependencies
target_link_libraries(Bar PRIVATE
    SableUI
    ${GLFW3_LIBRARIES}
    ${OPENGL_LIBRARIES}
    GLEW::GLEW
)