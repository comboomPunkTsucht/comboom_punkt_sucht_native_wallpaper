cmake_minimum_required(VERSION 3.20)
project(comboom_punkt_sucht_wallpaper_linux)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Platform detection
if(UNIX AND NOT APPLE)
    set(LINUX TRUE)
endif()

# --- GLFW ---
find_package(glfw3 3.4 REQUIRED)

# --- Vulkan ---
find_package(Vulkan REQUIRED)

# --- Linux-specific dependencies ---
if(LINUX)
    # pkg-config for AppIndicator
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(APPINDICATOR appindicator3-0.1)

    if(NOT APPINDICATOR_FOUND)
        message(WARNING "AppIndicator3 not found, system tray integration will be disabled")
    endif()
endif()

# --- Directories ---
set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(CORE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../core")
set(BUILD_LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../build")

# --- Source files ---
set(SOURCES
    ${SRC_DIR}/main.cpp
    ${SRC_DIR}/wallpaper_engine.cpp
    ${SRC_DIR}/window_manager.cpp
    ${SRC_DIR}/vulkan_renderer.cpp
)

if(LINUX AND APPINDICATOR_FOUND)
    list(APPEND SOURCES ${SRC_DIR}/linux/system_tray_linux.cpp)
endif()

# --- Shader compilation ---
set(SHADER_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/shaders")
file(MAKE_DIRECTORY ${SHADER_OUTPUT_DIR})

# Find glslc
find_program(GLSLC_EXECUTABLE glslc REQUIRED)

# Compile shaders
add_custom_command(
    OUTPUT ${SHADER_OUTPUT_DIR}/particle.vert.spv
    COMMAND ${GLSLC_EXECUTABLE} -fshader-stage=vertex
            ${SRC_DIR}/shaders/particle.vert.glsl
            -o ${SHADER_OUTPUT_DIR}/particle.vert.spv
    DEPENDS ${SRC_DIR}/shaders/particle.vert.glsl
    COMMENT "Compiling particle.vert.glsl"
)

add_custom_command(
    OUTPUT ${SHADER_OUTPUT_DIR}/particle.frag.spv
    COMMAND ${GLSLC_EXECUTABLE} -fshader-stage=fragment
            ${SRC_DIR}/shaders/particle.frag.glsl
            -o ${SHADER_OUTPUT_DIR}/particle.frag.spv
    DEPENDS ${SRC_DIR}/shaders/particle.frag.glsl
    COMMENT "Compiling particle.frag.glsl"
)

# --- Create executable ---
add_executable(comboom_punkt_sucht_wallpaper
    ${SOURCES}
    ${SHADER_OUTPUT_DIR}/particle.vert.spv
    ${SHADER_OUTPUT_DIR}/particle.frag.spv
)

# --- Include directories ---
target_include_directories(comboom_punkt_sucht_wallpaper PRIVATE
    ${SRC_DIR}
    ${CORE_DIR}
    ${Vulkan_INCLUDE_DIRS}
)

# --- Link libraries ---
target_link_libraries(comboom_punkt_sucht_wallpaper
    glfw
    Vulkan::Vulkan
)

# AppIndicator on Linux
if(LINUX AND APPINDICATOR_FOUND)
    target_include_directories(comboom_punkt_sucht_wallpaper PRIVATE
        ${APPINDICATOR_INCLUDE_DIRS}
    )
    target_link_directories(comboom_punkt_sucht_wallpaper PRIVATE
        ${APPINDICATOR_LIBRARY_DIRS}
    )
    target_link_libraries(comboom_punkt_sucht_wallpaper
        ${APPINDICATOR_LIBRARIES}
    )
    target_compile_definitions(comboom_punkt_sucht_wallpaper PRIVATE
        HAVE_APPINDICATOR=1
    )
endif()

# Link C-core library
target_link_libraries(comboom_punkt_sucht_wallpaper
    ${BUILD_LIB_DIR}/libcbps_we_core_linux.a
)

# --- Compiler flags ---
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    target_compile_options(comboom_punkt_sucht_wallpaper PRIVATE
        -Wall -Wextra -Wpedantic
        -O3
    )
endif()

# --- Output directory ---
set_target_properties(comboom_punkt_sucht_wallpaper PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# --- Install target (optional) ---
install(TARGETS comboom_punkt_sucht_wallpaper
    RUNTIME DESTINATION bin
)

message(STATUS "✓ Linux Vulkan wallpaper app configured")
message(STATUS "  - Target: ${CMAKE_CURRENT_BINARY_DIR}/bin/comboom_punkt_sucht_wallpaper")
message(STATUS "  - C++ Standard: C++23")
message(STATUS "  - Rendering: Vulkan")
message(STATUS "  - AppIndicator: ${APPINDICATOR_FOUND}")
