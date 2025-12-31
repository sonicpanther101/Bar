cmake_minimum_required(VERSION 3.15)
project(Bar)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(PROTOCOL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/protocols")
include_directories(${PROTOCOL_DIR})

find_package(PkgConfig REQUIRED)

# Pull in ALL standard Wayland protocols + core libs
pkg_check_modules(WAYLAND REQUIRED 
    wayland-client 
    wayland-egl 
    wayland-cursor 
    xkbcommon
    egl 
    glesv2
)

find_package(GLEW REQUIRED)

add_definitions(-DGLEW_EGL -DGLEW_NO_GLU)

file(GLOB SOURCE "*.cpp" "*.h")
add_executable(Bar
    ${SOURCE}
    ${PROTOCOL_DIR}/wlr-layer-shell-unstable-v1-protocol.c
    ${PROTOCOL_DIR}/xdg-shell-protocol.c
)

target_include_directories(Bar PRIVATE
    ${WAYLAND_INCLUDE_DIRS}
    ${EGL_INCLUDE_DIRS}
    ${GLES2_INCLUDE_DIRS}
)

# Comprehensive linking - covers all protocol deps
target_link_libraries(Bar PRIVATE
    ${WAYLAND_LIBRARIES}
    wayland-client
    wayland-egl
    wayland-cursor
    xkbcommon
    EGL
    GLESv2
    GLEW::GLEW
)

# Force link order and add any missing deps from pkg-config
target_link_options(Bar PRIVATE 
    ${WAYLAND_LDFLAGS}
    -Wl,--no-as-needed  # NixOS linker quirk
)
