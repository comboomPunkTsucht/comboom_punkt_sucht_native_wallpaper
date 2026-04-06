cmake_minimum_required(VERSION 3.20)
project(comboom_punkt_sucht_wallpaper_windows)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# --- GLFW ---
find_package(glfw3 3.3 REQUIRED)

# --- Vulkan ---
find_package(Vulkan REQUIRED)

# --- Directories ---
set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(CORE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../core")
set(BUILD_LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../build")
set(LINUX_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../Linux/src")

# --- Source files (shared from Linux, platform-specific implementations) ---
set(SOURCES
    ${LINUX_SRC_DIR}/main.cpp
    ${LINUX_SRC_DIR}/wallpaper_engine.cpp
    ${LINUX_SRC_DIR}/window_manager.cpp
    ${LINUX_SRC_DIR}/vulkan_renderer.cpp
    ${SRC_DIR}/windows/system_tray_windows.cpp
)

# --- Shader compilation ---
set(SHADER_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/shaders")
file(MAKE_DIRECTORY ${SHADER_OUTPUT_DIR})

# Find glslc or glslangValidator
find_program(GLSLC_EXECUTABLE glslc)
if(NOT GLSLC_EXECUTABLE)
    find_program(GLSLC_EXECUTABLE glslangValidator REQUIRED)
    set(GLSLC_ARGS "-V")  # glslangValidator needs -V flag for Vulkan
else()
    set(GLSLC_ARGS "")
endif()

# Compile shaders
add_custom_command(
    OUTPUT ${SHADER_OUTPUT_DIR}/particle.vert.spv
    COMMAND ${GLSLC_EXECUTABLE} ${GLSLC_ARGS} -fshader-stage=vertex
            ${LINUX_SRC_DIR}/shaders/particle.vert.glsl
            -o ${SHADER_OUTPUT_DIR}/particle.vert.spv
    DEPENDS ${LINUX_SRC_DIR}/shaders/particle.vert.glsl
    COMMENT "Compiling particle.vert.glsl"
)

add_custom_command(
    OUTPUT ${SHADER_OUTPUT_DIR}/particle.frag.spv
    COMMAND ${GLSLC_EXECUTABLE} ${GLSLC_ARGS} -fshader-stage=fragment
            ${LINUX_SRC_DIR}/shaders/particle.frag.glsl
            -o ${SHADER_OUTPUT_DIR}/particle.frag.spv
    DEPENDS ${LINUX_SRC_DIR}/shaders/particle.frag.glsl
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
    ${LINUX_SRC_DIR}
    ${SRC_DIR}
    ${CORE_DIR}
    ${Vulkan_INCLUDE_DIRS}
)

# --- Link libraries ---
target_link_libraries(comboom_punkt_sucht_wallpaper
    glfw
    Vulkan::Vulkan
)

# Windows-specific libraries
target_link_libraries(comboom_punkt_sucht_wallpaper
    shell32  # For system tray (Shell_NotifyIcon)
)

# Link C-core library
target_link_libraries(comboom_punkt_sucht_wallpaper
    ${BUILD_LIB_DIR}/libcbps_we_core_win.a
)

# --- Platform definition ---
target_compile_definitions(comboom_punkt_sucht_wallpaper PRIVATE
    _WIN32_WINNT=0x0A00  # Windows 10+
)

# --- Compiler flags ---
if(MSVC)
    target_compile_options(comboom_punkt_sucht_wallpaper PRIVATE
        /W4 /EHsc /O2
    )
else()
    target_compile_options(comboom_punkt_sucht_wallpaper PRIVATE
        -Wall -Wextra -Wpedantic -O3
    )
endif()

# --- Output directory ---
set_target_properties(comboom_punkt_sucht_wallpaper PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    WIN32_EXECUTABLE ON
)

# --- Install target (optional) ---
install(TARGETS comboom_punkt_sucht_wallpaper
    RUNTIME DESTINATION bin
)

message(STATUS "✓ Windows Vulkan wallpaper app configured")
message(STATUS "  - Target: ${CMAKE_CURRENT_BINARY_DIR}/bin/comboom_punkt_sucht_wallpaper.exe")
message(STATUS "  - C++ Standard: C++23")
message(STATUS "  - Rendering: Vulkan")
message(STATUS "  - Platform: Windows 10+")
