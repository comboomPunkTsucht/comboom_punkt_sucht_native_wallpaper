# --- Konfiguration ---
CC := "clang"
CXX := "clang++"
SRC := "core/cbps_wallpaper_engine_core.c"
BUILD_DIR := "build"
RAYLIB_DIR := "thirdparty/raylib"

# MinGW Sysroot auto-detection (macOS vs Linux)
# On macOS: Homebrew Cellar path
# On Linux: Common Ubuntu/Debian paths, or empty if not found
MINGW_SYSROOT := if os() == "macos" {
    "/opt/homebrew/Cellar/mingw-w64/14.0.0/toolchain-x86_64"
} else {
    if path_exists("/usr/x86_64-w64-mingw32") {
        "/usr/x86_64-w64-mingw32"
    } else {
        ""
    }
}

MINGW_SYSROOT_OVERRIDE := env_var_or_default("MINGW_SYSROOT", MINGW_SYSROOT)

# --- Ausgabe-Dateien ---
WASM_OUT     := BUILD_DIR + "/cbps_we_core.wasm"
MAC_STATIC   := BUILD_DIR + "/libcbps_we_core_mac.a"
WIN_STATIC   := BUILD_DIR + "/libcbps_we_core_win.a"
LINUX_STATIC := BUILD_DIR + "/libcbps_we_core_linux.a"

# Raylib output paths
RAYLIB_LINUX_X64  := BUILD_DIR + "/libraylib_linux_x64.a"
RAYLIB_LINUX_ARM64  := BUILD_DIR + "/libraylib_linux_arm64.a"
RAYLIB_WIN_X64    := BUILD_DIR + "/libraylib_win_x64.a"
RAYLIB_WIN_ARM64  := BUILD_DIR + "/libraylib_win_arm64.a"

# App output paths
APP_LINUX_X64  := BUILD_DIR + "/comboom-sucht-linux-x64"
APP_LINUX_ARM64  := BUILD_DIR + "/comboom-sucht-linux-arm64"
APP_WIN_X64    := BUILD_DIR + "/comboom-sucht-windows-x64.exe"
APP_WIN_ARM64  := BUILD_DIR + "/comboom-sucht-windows-arm64.exe"

# Standard-Task: Zeigt das Hilfe-Menü an
default:
    @echo ""
    @echo "  ╔════════════════════════════════════════════════════════╗"
    @echo "  ║   comboom. sucht - Native Wallpaper (Raylib Edition)  ║"
    @echo "  ╚════════════════════════════════════════════════════════╝"
    @echo ""
    @echo "  📚 C-Core Libraries (Statisch)"
    @echo "  ━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
    @echo "    just wasm         - Build WebAssembly (.wasm)"
    @echo "    just mac-static   - Build macOS Static Library (.a)"
    @echo "    just win-static   - Build Windows Static Library (.a)"
    @echo "    just linux-static - Build Linux Static Library (.a)"
    @echo ""
    @echo "  🚀 Native Apps (Raylib) - Clang 18"
    @echo "  ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
    @echo "    just linux-raylib         - Build Linux (x64, X11/Wayland, Raylib)"
    @echo "    just linux-raylib-arm64   - Build Linux (ARM64 cross-compile, Raylib)"
    @echo ""
    @echo "    just windows-raylib       - Build Windows (x64, MinGW, Raylib)"
    @echo "    (Windows ARM64 support pending)"
    @echo ""
    @echo "  🎯 Build Shortcuts"
    @echo "  ━━━━━━━━━━━━━━━━"
    @echo "    just all          - Build all libraries"
    @echo "    just apps         - Build all native apps (x64 only)"
    @echo "    just apps-all     - Build all apps & architectures"
    @echo ""
    @echo "  🧹 Utility"
    @echo "  ━━━━━━━"
    @echo "    just clean        - Delete build artifacts"
    @echo ""

@prepare:
    mkdir -p {{BUILD_DIR}}

# --- 1. WebAssembly (unchanged) ---
@wasm: prepare
	echo "🚀 Baue WebAssembly (.wasm)..."
	{{CC}} --target=wasm32-unknown-unknown -nostdlib -fno-builtin -O3 -flto -Wl,--no-entry -Wl,--export-dynamic -Wl,--export-table -Wl,--growable-table -Wl,--allow-undefined -Wl,--export=cbps_engine_create -Wl,--export=cbps_engine_update -Wl,--export=cbps_engine_destroy -Wl,--export=cbps_engine_set_seed -Wl,--export=__heap_base -o {{WASM_OUT}} {{SRC}}
	echo "📦 Kopiere .wasm in den SvelteKit static Ordner..."
	cp {{WASM_OUT}} ./WASM/static/cbps_we_core.wasm
	echo "✅ Fertig!"

# --- 2. macOS Static (unchanged) ---
@mac-static: prepare
    echo "🍎 Baue macOS Static (.a)..."
    {{CC}} -O3 -c {{SRC}} -o {{BUILD_DIR}}/mac.o
    ar rcs {{MAC_STATIC}} {{BUILD_DIR}}/mac.o
    rm {{BUILD_DIR}}/mac.o
    echo "✅ Fertig: {{MAC_STATIC}}"

# --- 3. Windows Static (unchanged) ---
@win-static: prepare
	echo "🪟 Baue Windows Static (.a) mit Clang..."
	{{CC}} -O3 -c {{SRC}} -o {{BUILD_DIR}}/win.o
	ar rcs {{WIN_STATIC}} {{BUILD_DIR}}/win.o
	rm {{BUILD_DIR}}/win.o
	echo "✅ Fertig: {{WIN_STATIC}}"

# --- 4. Linux Static (unchanged) ---
@linux-static: prepare
    echo "🐧 Baue Linux Static (.a)..."
    {{CC}} -O3 -c {{SRC}} -o {{BUILD_DIR}}/linux.o
    ar rcs {{LINUX_STATIC}} {{BUILD_DIR}}/linux.o
    rm {{BUILD_DIR}}/linux.o
    echo "✅ Fertig: {{LINUX_STATIC}}"

# --- Build all static libraries ---
@libs_all: wasm mac-static win-static linux-static
    echo ""
    echo "✅ All C-Core libraries built!"
    echo ""

# --- Raylib Compilation via Makefile (Linux x64) ---
@build-raylib-linux-x64: prepare
    echo "📦 Compiling Raylib for Linux (x64) via Makefile..."
    mkdir -p {{BUILD_DIR}}/raylib_linux_x64
    cd {{RAYLIB_DIR}}/src && \
    make \
        CC={{CC}} \
        PLATFORM=PLATFORM_DESKTOP \
        GRAPHICS=GRAPHICS_API_OPENGL_43 \
        LDLIBS="" \
        -j$(nproc) && \
    cp libraylib.a ../../../{{RAYLIB_LINUX_X64}} && \
    make clean
    echo "✅ Raylib static library: {{RAYLIB_LINUX_X64}}"

# --- Raylib Compilation via Makefile (Linux ARM64 cross-compile) ---
@build-raylib-linux-arm64: prepare
    echo "📦 Compiling Raylib for Linux (ARM64) via Makefile (cross-compile)..."
    mkdir -p {{BUILD_DIR}}/raylib_linux_arm64
    cd {{RAYLIB_DIR}}/src && \
    make \
        CC="clang --target=aarch64-linux-gnu" \
        PLATFORM=PLATFORM_DESKTOP \
        GRAPHICS=GRAPHICS_API_OPENGL_43 \
        LDLIBS="" \
        -j$(nproc) && \
    cp libraylib.a ../../../{{RAYLIB_LINUX_ARM64}} && \
    make clean
    echo "✅ Raylib static library: {{RAYLIB_LINUX_ARM64}}"

# --- Raylib Compilation via Makefile (Windows x64) ---
# NOTE: Windows builds must be compiled on Linux/Ubuntu (see GitHub Actions workflow)
# MinGW-w64 on macOS has limited Windows header support, use Linux for Windows cross-compilation
@build-raylib-windows-x64: prepare
    #!/usr/bin/env bash
    if [[ "$(uname)" == "Darwin" ]]; then
        echo "❌ ERROR: Windows builds cannot be compiled on macOS"
        echo ""
        echo "MinGW-w64 on macOS has incomplete Windows SDK files."
        echo "This is a known limitation of the macOS MinGW installation."
        echo ""
        echo "👉 Windows builds are compiled on Linux (GitHub Actions CI/CD)"
        echo "   See .github/workflows/release-native-apps.yml"
        echo ""
        echo "For local development on macOS:"
        echo "  • Build Linux apps: just linux-raylib"
        echo ""
        exit 1
    else
        echo "📦 Compiling Raylib for Windows (x64) via Makefile (MinGW)..."
        mkdir -p {{BUILD_DIR}}/raylib_win_x64
        cd {{RAYLIB_DIR}}/src && \
        make \
            CC="clang --target=x86_64-w64-mingw32" \
            PLATFORM=PLATFORM_DESKTOP \
            GRAPHICS=GRAPHICS_API_OPENGL_43 \
            GLFW_USE_X11=0 \
            GLFW_USE_WAYLAND=0 \
            CFLAGS="-D_GLFW_WIN32 -DGL_SILENCE_DEPRECATION -DUNICODE -D_UNICODE" \
            LDLIBS="" \
            -j$(nproc) && \
        cp libraylib.a ../../../{{RAYLIB_WIN_X64}} && \
        make clean
        echo "✅ Raylib static library: {{RAYLIB_WIN_X64}}"
    fi

# --- Raylib Compilation via Makefile (Windows ARM64) ---
# NOTE: Temporarily disabled - MinGW-w64 ARM64 support in Ubuntu is limited
# Uncomment and use when better cross-compilation toolchains are available
# @build-raylib-windows-arm64: prepare
#     echo "📦 Compiling Raylib for Windows (ARM64) via Makefile (cross-compile)..."
#     mkdir -p {{BUILD_DIR}}/raylib_win_arm64
#     cd {{RAYLIB_DIR}}/src && \
#     make \
#         CC="clang --target=aarch64-w64-mingw32" \
#         PLATFORM=PLATFORM_DESKTOP \
#         GRAPHICS=GRAPHICS_API_OPENGL_43 \
#         GLFW_USE_X11=0 \
#         GLFW_USE_WAYLAND=0 \
#         CFLAGS="-D_GLFW_WIN32 -DGL_SILENCE_DEPRECATION" \
#         LDLIBS="" \
#         -j$(nproc) && \
#     cp libraylib.a ../../../{{RAYLIB_WIN_ARM64}} && \
#     make clean
#     echo "✅ Raylib static library: {{RAYLIB_WIN_ARM64}}"

# --- Windows Raylib App (ARM64 cross-compile) ---
# NOTE: Temporarily disabled - use when build-raylib-windows-arm64 is re-enabled
# @windows-raylib-arm64: win-static build-raylib-windows-arm64
#     echo "🪟 Compiling Windows Raylib App (ARM64 cross-compile, MinGW)..."
#     mkdir -p {{BUILD_DIR}}/app_win_arm64
#     # Compile C core
#     {{CXX}} --target=aarch64-w64-mingw32 -std=c++23 -O3 -c {{SRC}} \
#         -o {{BUILD_DIR}}/app_win_arm64/cbps_core.o
#     # Compile C++ app files
#     {{CXX}} --target=aarch64-w64-mingw32 -std=c++23 -O3 -fPIC \
#         -I{{RAYLIB_DIR}}/src -Icore \
#         -c Linux-Windows-shared/src/main_raylib.cpp \
#         -o {{BUILD_DIR}}/app_win_arm64/main.o
#     {{CXX}} --target=aarch64-w64-mingw32 -std=c++23 -O3 -fPIC \
#         -I{{RAYLIB_DIR}}/src -Icore \
#         -c Linux-Windows-shared/src/wallpaper_app.cpp \
#         -o {{BUILD_DIR}}/app_win_arm64/wallpaper_app.o
#     {{CXX}} --target=aarch64-w64-mingw32 -std=c++23 -O3 -fPIC \
#         -I{{RAYLIB_DIR}}/src -Icore \
#         -c Linux-Windows-shared/src/renderer_raylib.cpp \
#         -o {{BUILD_DIR}}/app_win_arm64/renderer.o
#     {{CXX}} --target=aarch64-w64-mingw32 -std=c++23 -O3 -fPIC \
#         -I{{RAYLIB_DIR}}/src -Icore \
#         -c Linux-Windows-shared/src/assets_loader.cpp \
#         -o {{BUILD_DIR}}/app_win_arm64/assets_loader.o
#     {{CXX}} --target=aarch64-w64-mingw32 -std=c++23 -O3 -fPIC \
#         -I{{RAYLIB_DIR}}/src -Icore -c Linux-Windows-shared/src/system_tray_windows.cpp \
#         -o {{BUILD_DIR}}/app_win_arm64/system_tray_windows.o
#     # Link with static libraries and Windows API
#     {{CXX}} --target=aarch64-w64-mingw32 -O3 {{BUILD_DIR}}/app_win_arm64/*.o \
#         {{RAYLIB_WIN_ARM64}} {{WIN_STATIC}} \
#         -lopengl32 -lgdi32 -luser32 -lshell32 -lole32 -lwinmm \
#         -static-libstdc++ -static-libgcc \
#         -o {{APP_WIN_ARM64}}
#     echo "✅ App built: {{APP_WIN_ARM64}}"

# --- Embed Assets with C++23 #embed ---
@embed-assets: prepare
    echo "📦 Embedding assets with C++23 #embed..."
    echo "✅ Assets embedded via #embed in source code (no separate step needed)"

# --- Linux Raylib App (x64) ---
@linux-raylib: linux-static build-raylib-linux-x64
    echo "🐧 Compiling Linux Raylib App (x64)..."
    mkdir -p {{BUILD_DIR}}/app_linux_x64
    # Compile C core
    {{CXX}} -std=c++23 -O3 -c {{SRC}} \
        -o {{BUILD_DIR}}/app_linux_x64/cbps_core.o
    # Compile C++ app files
    {{CXX}} -std=c++23 -O3 -fPIC \
        -I{{RAYLIB_DIR}}/src -Icore \
        -c Linux-Windows-shared/src/main_raylib.cpp \
        -o {{BUILD_DIR}}/app_linux_x64/main.o
    {{CXX}} -std=c++23 -O3 -fPIC \
        -I{{RAYLIB_DIR}}/src -Icore \
        -c Linux-Windows-shared/src/wallpaper_app.cpp \
        -o {{BUILD_DIR}}/app_linux_x64/wallpaper_app.o
    {{CXX}} -std=c++23 -O3 -fPIC \
        -I{{RAYLIB_DIR}}/src -Icore \
        -c Linux-Windows-shared/src/renderer_raylib.cpp \
        -o {{BUILD_DIR}}/app_linux_x64/renderer.o
    {{CXX}} -std=c++23 -O3 -fPIC \
        -I{{RAYLIB_DIR}}/src -Icore \
        -c Linux-Windows-shared/src/assets_loader.cpp \
        -o {{BUILD_DIR}}/app_linux_x64/assets_loader.o
    # Platform-specific tray
    #!/bin/bash
    set -e
    if [[ "$OSTYPE" == "darwin"* ]]; then \
        echo "ℹ️  Compiling with macOS stub system tray (for testing only)..."; \
        {{CXX}} -std=c++23 -O3 -fPIC \
            -I{{RAYLIB_DIR}}/src -Icore \
            -c Linux-Windows-shared/src/system_tray_macos_stub.cpp \
            -o {{BUILD_DIR}}/app_linux_x64/system_tray.o; \
    else \
        {{CXX}} -std=c++23 -O3 -fPIC \
            -I{{RAYLIB_DIR}}/src -Icore \
            -c Linux-Windows-shared/src/system_tray_linux.cpp \
            -o {{BUILD_DIR}}/app_linux_x64/system_tray.o; \
    fi
    # Link (try with full dependencies first, fallback to minimal on macOS)
    {{CXX}} -O3 {{BUILD_DIR}}/app_linux_x64/*.o \
        {{RAYLIB_LINUX_X64}} {{LINUX_STATIC}} \
        -lX11 -lGLX -lGL -ldl -lpthread \
        -lappindicator3 -lgtk-3 -lglib-2.0 -lgobject-2.0 \
        -static-libstdc++ -static-libgcc \
        -o {{APP_LINUX_X64}} 2>/dev/null || \
    {{CXX}} -O3 {{BUILD_DIR}}/app_linux_x64/*.o \
        {{RAYLIB_LINUX_X64}} {{LINUX_STATIC}} \
        -lX11 -lGLX -lGL -ldl -lpthread \
        -static-libstdc++ -static-libgcc \
        -o {{APP_LINUX_X64}}
    echo "✅ App built: {{APP_LINUX_X64}}"

# --- Linux Raylib App (ARM64 cross-compile) ---
@linux-raylib-arm64: linux-static build-raylib-linux-arm64
    echo "🐧 Compiling Linux Raylib App (ARM64 cross-compile)..."
    mkdir -p {{BUILD_DIR}}/app_linux_arm64
    # Compile C core with aarch64 target
    {{CXX}} --target=aarch64-linux-gnu -std=c++23 -O3 -c {{SRC}} \
        -o {{BUILD_DIR}}/app_linux_arm64/cbps_core.o
    # Compile C++ app files
    {{CXX}} --target=aarch64-linux-gnu -std=c++23 -O3 -fPIC \
        -I{{RAYLIB_DIR}}/src -Icore \
        -c Linux-Windows-shared/src/main_raylib.cpp \
        -o {{BUILD_DIR}}/app_linux_arm64/main.o
    {{CXX}} --target=aarch64-linux-gnu -std=c++23 -O3 -fPIC \
        -I{{RAYLIB_DIR}}/src -Icore \
        -c Linux-Windows-shared/src/wallpaper_app.cpp \
        -o {{BUILD_DIR}}/app_linux_arm64/wallpaper_app.o
    {{CXX}} --target=aarch64-linux-gnu -std=c++23 -O3 -fPIC \
        -I{{RAYLIB_DIR}}/src -Icore \
        -c Linux-Windows-shared/src/renderer_raylib.cpp \
        -o {{BUILD_DIR}}/app_linux_arm64/renderer.o
    {{CXX}} --target=aarch64-linux-gnu -std=c++23 -O3 -fPIC \
        -I{{RAYLIB_DIR}}/src -Icore \
        -c Linux-Windows-shared/src/assets_loader.cpp \
        -o {{BUILD_DIR}}/app_linux_arm64/assets_loader.o
    {{CXX}} --target=aarch64-linux-gnu -std=c++23 -O3 -fPIC \
        -I{{RAYLIB_DIR}}/src -Icore \
        -c Linux-Windows-shared/src/system_tray_linux.cpp \
        -o {{BUILD_DIR}}/app_linux_arm64/system_tray_linux.o
    # Link with static libraries
    {{CXX}} --target=aarch64-linux-gnu -O3 {{BUILD_DIR}}/app_linux_arm64/*.o \
        {{RAYLIB_LINUX_ARM64}} {{LINUX_STATIC}} \
        --sysroot=/usr/aarch64-linux-gnu \
        -lX11 -lGLX -lGL -ldl -lpthread \
        -lappindicator3 -lgtk-3 -lglib-2.0 -lgobject-2.0 \
        -static-libstdc++ -static-libgcc \
        -o {{APP_LINUX_ARM64}}
    echo "✅ App built: {{APP_LINUX_ARM64}}"

# --- Windows Raylib App (x64) ---
@windows-raylib: win-static build-raylib-windows-x64
    echo "🪟 Compiling Windows Raylib App (x64 MinGW)..."
    mkdir -p {{BUILD_DIR}}/app_win_x64
    # Compile C core
    {{CXX}} --target=x86_64-w64-mingw32 -std=c++23 -O3 -c {{SRC}} \
        -o {{BUILD_DIR}}/app_win_x64/cbps_core.o
    # Compile C++ app files
    {{CXX}} --target=x86_64-w64-mingw32 -std=c++23 -O3 -fPIC \
        -I{{RAYLIB_DIR}}/src -Icore \
        -c Linux-Windows-shared/src/main_raylib.cpp \
        -o {{BUILD_DIR}}/app_win_x64/main.o
    {{CXX}} --target=x86_64-w64-mingw32 -std=c++23 -O3 -fPIC \
        -I{{RAYLIB_DIR}}/src -Icore \
        -c Linux-Windows-shared/src/wallpaper_app.cpp \
        -o {{BUILD_DIR}}/app_win_x64/wallpaper_app.o
    {{CXX}} --target=x86_64-w64-mingw32 -std=c++23 -O3 -fPIC \
        -I{{RAYLIB_DIR}}/src -Icore \
        -c Linux-Windows-shared/src/renderer_raylib.cpp \
        -o {{BUILD_DIR}}/app_win_x64/renderer.o
    {{CXX}} --target=x86_64-w64-mingw32 -std=c++23 -O3 -fPIC \
        -I{{RAYLIB_DIR}}/src -Icore \
        -c Linux-Windows-shared/src/assets_loader.cpp \
        -o {{BUILD_DIR}}/app_win_x64/assets_loader.o
    {{CXX}} --target=x86_64-w64-mingw32 -std=c++23 -O3 -fPIC \
        -I{{RAYLIB_DIR}}/src -Icore \
        -c Linux-Windows-shared/src/system_tray_windows.cpp \
        -o {{BUILD_DIR}}/app_win_x64/system_tray_windows.o
    # Link with static libraries and Windows API
    {{CXX}} --target=x86_64-w64-mingw32 -O3 {{BUILD_DIR}}/app_win_x64/*.o \
        {{RAYLIB_WIN_X64}} {{WIN_STATIC}} \
        -lopengl32 -lgdi32 -luser32 -lshell32 -lole32 -lwinmm \
        -static-libstdc++ -static-libgcc \
        -o {{APP_WIN_X64}}
    echo "✅ App built: {{APP_WIN_X64}}"

# --- Build all apps ---
@apps: linux-raylib windows-raylib
    echo ""
    echo "✅ All default apps built (Linux x64, Windows x64)!"
    echo ""

# --- Package Linux app with assets ---
@package-linux: linux-raylib
    echo "📦 Packaging Linux app with assets..."
    mkdir -p {{BUILD_DIR}}/dist/comboom-sucht-linux-x64
    cp {{APP_LINUX_X64}} {{BUILD_DIR}}/dist/comboom-sucht-linux-x64/
    mkdir -p {{BUILD_DIR}}/dist/comboom-sucht-linux-x64/assets
    cp WASM/static/Logo.png {{BUILD_DIR}}/dist/comboom-sucht-linux-x64/assets/
    cp -r WASM/static/fonts {{BUILD_DIR}}/dist/comboom-sucht-linux-x64/assets/ 2>/dev/null || true
    cp -r WASM/static/pictures {{BUILD_DIR}}/dist/comboom-sucht-linux-x64/assets/ 2>/dev/null || true
    cp ASSETS_README.md {{BUILD_DIR}}/dist/comboom-sucht-linux-x64/
    cd {{BUILD_DIR}}/dist && tar -czf ../comboom-sucht-linux-x64.tar.gz comboom-sucht-linux-x64/
    rm -rf {{BUILD_DIR}}/dist
    echo "✅ Package: {{BUILD_DIR}}/comboom-sucht-linux-x64.tar.gz"

# --- Package Windows app with assets ---
@package-windows: windows-raylib
    echo "📦 Packaging Windows app with assets..."
    mkdir -p {{BUILD_DIR}}/dist/comboom-sucht-windows-x64
    cp {{APP_WIN_X64}} {{BUILD_DIR}}/dist/comboom-sucht-windows-x64/
    mkdir -p {{BUILD_DIR}}/dist/comboom-sucht-windows-x64/assets
    cp WASM/static/Logo.png {{BUILD_DIR}}/dist/comboom-sucht-windows-x64/assets/
    cp -r WASM/static/fonts {{BUILD_DIR}}/dist/comboom-sucht-windows-x64/assets/ 2>/dev/null || true
    cp -r WASM/static/pictures {{BUILD_DIR}}/dist/comboom-sucht-windows-x64/assets/ 2>/dev/null || true
    cp ASSETS_README.md {{BUILD_DIR}}/dist/comboom-sucht-windows-x64/
    cd {{BUILD_DIR}}/dist && zip -r ../comboom-sucht-windows-x64.zip comboom-sucht-windows-x64/
    rm -rf {{BUILD_DIR}}/dist
    echo "✅ Package: {{BUILD_DIR}}/comboom-sucht-windows-x64.zip"

# --- Package all distributions ---
@package-all: package-linux linux-raylib-arm64 package-windows
    echo ""
    echo "🍎 Preparing Linux ARM64 package..."
    mkdir -p {{BUILD_DIR}}/dist/comboom-sucht-linux-arm64
    cp {{APP_LINUX_ARM64}} {{BUILD_DIR}}/dist/comboom-sucht-linux-arm64/
    mkdir -p {{BUILD_DIR}}/dist/comboom-sucht-linux-arm64/assets
    cp WASM/static/Logo.png {{BUILD_DIR}}/dist/comboom-sucht-linux-arm64/assets/
    cp -r WASM/static/fonts {{BUILD_DIR}}/dist/comboom-sucht-linux-arm64/assets/ 2>/dev/null || true
    cp -r WASM/static/pictures {{BUILD_DIR}}/dist/comboom-sucht-linux-arm64/assets/ 2>/dev/null || true
    cp ASSETS_README.md {{BUILD_DIR}}/dist/comboom-sucht-linux-arm64/
    cd {{BUILD_DIR}}/dist && tar -czf ../comboom-sucht-linux-arm64.tar.gz comboom-sucht-linux-arm64/
    rm -rf {{BUILD_DIR}}/dist
    echo "✅ Package: {{BUILD_DIR}}/comboom-sucht-linux-arm64.tar.gz"
    echo ""
    echo "✅ All packages ready in {{BUILD_DIR}}/"

# --- Build all apps and architectures ---
@apps-all: linux-raylib linux-raylib-arm64 windows-raylib
    echo ""
    echo "✅ All supported apps & architectures built!"
    echo ""
    echo "📦 Artifacts:"
    echo "  {{APP_LINUX_X64}}"
    echo "  {{APP_LINUX_ARM64}}"
    echo "  {{APP_WIN_X64}}"
    echo ""

# --- Build all (libraries only) ---
@all: libs_all
    echo ""
    echo "✅ All C-Core libraries built!"
    echo ""

# --- Clean up ---
@clean:
    echo "🧹 Cleaning build artifacts..."
    rm -rf {{BUILD_DIR}}
    cd {{RAYLIB_DIR}}/src && make clean 2>/dev/null || true
    echo "✨ Clean!"

# --- Info/status ---
@info:
    @echo ""
    @echo "  ℹ️  comboom. sucht - Raylib Native Edition"
    @echo "  ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
    @echo ""
    @echo "  Build System: Justfile + Clang (C++23)"
    @echo "  Graphics: Raylib (OpenGL) - compiled via native Makefile"
    @echo "  System Tray:"
    @echo "    • Linux: AppIndicator3 + GTK"
    @echo "    • Windows: Win32 API"
    @echo "  Asset Embedding: C++23 #embed"
    @echo ""
    @echo "  Platforms:"
    @echo "    • Linux (x64 & ARM64) - X11/Wayland"
    @echo "    • Windows (x64) - MinGW [ARM64 support pending]"
    @echo ""

# --- Shorthand aliases ---
alias l := linux-raylib
alias la := linux-raylib-arm64
alias w := windows-raylib
alias a := apps
alias aa := apps-all
alias c := clean
