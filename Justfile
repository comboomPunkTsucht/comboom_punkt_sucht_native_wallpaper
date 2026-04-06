# --- Konfiguration ---
# Defaults: can be overridden via environment (e.g., CC=clang-18 just libs_linux)
CC := "clang"
CXX := "clang++"
SRC := "core/cbps_wallpaper_engine_core.c"
BUILD_DIR := "build"
TOOLCHAIN := "" # Add this line

# --- Ausgabe-Dateien ---
WASM_OUT     := BUILD_DIR + "/cbps_we_core.wasm"
MAC_STATIC   := BUILD_DIR + "/libcbps_we_core_mac.a"
WIN_STATIC   := BUILD_DIR + "/libcbps_we_core_win.a"
LINUX_STATIC := BUILD_DIR + "/libcbps_we_core_linux.a"

# Standard-Task: Zeigt das Hilfe-Menü an
default:
    @echo ""
    @echo "  ╔════════════════════════════════════════════════════════╗"
    @echo "  ║        comboom. sucht - Multi-Platform Build          ║"
    @echo "  ╚════════════════════════════════════════════════════════╝"
    @echo ""
    @echo "  📚 C-Core Libraries (Statisch)"
    @echo "  ━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
    @echo "    just wasm         - Build WebAssembly (.wasm)"
    @echo "    just mac-static   - Build macOS Static Library (.a)"
    @echo "    just win-static   - Build Windows Static Library (.a)"
    @echo "    just linux-static - Build Linux Static Library (.a)"
    @echo "    just libs_all     - Build all libraries"
    @echo ""
    @echo "  🚀 Native Apps (Vulkan/Metal) - Clang 18"
    @echo "  ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
    @echo "    just linux-vulkan      - Build Linux Vulkan (x86_64, X11/Wayland)"
    @echo "    just linux-vulkan-arm  - Build Linux Vulkan (ARM64 cross-compile)"
    @echo ""
    @echo "    just windows-vulkan    - Build Windows Vulkan (x86_64, Clang/LLVM)"
    @echo "    just windows-vulkan-arm - Build Windows Vulkan (ARM64 cross-compile)"
    @echo ""
    @echo "    just macos-metal   - Build macOS Metal (ARM64 Apple Silicon)"
    @echo ""
    @echo "  🎯 Build Shortcuts"
    @echo "  ━━━━━━━━━━━━━━━━"
    @echo "    just all           - Build all libraries + macOS app"
    @echo "    just native        - Build all native apps (recommended)"
    @echo ""
    @echo "  🧹 Utility"
    @echo "  ━━━━━━━━"
    @echo "    just clean         - Delete build artifacts"
    @echo "    just rebuild       - Clean + rebuild all libraries + macOS"
    @echo ""

@prepare:
    mkdir -p {{BUILD_DIR}}

# --- 1. WebAssembly ---
@wasm: prepare
	echo "🚀 Baue WebAssembly (.wasm)..."
	{{CC}} --target=wasm32-unknown-unknown -nostdlib -fno-builtin -O3 -flto -Wl,--no-entry -Wl,--export-dynamic -Wl,--export-table -Wl,--growable-table -Wl,--allow-undefined -Wl,--export=cbps_engine_create -Wl,--export=cbps_engine_update -Wl,--export=cbps_engine_destroy -Wl,--export=cbps_engine_set_seed -Wl,--export=__heap_base -o {{WASM_OUT}} {{SRC}}
	echo "📦 Kopiere .wasm in den SvelteKit static Ordner..."
	cp {{WASM_OUT}} ./WASM/static/cbps_we_core.wasm
	echo "✅ Fertig!"

# --- 2. macOS ---
@mac-static: prepare
    echo "🍎 Baue macOS Static (.a)..."
    {{CC}} -O3 -c {{SRC}} -o {{BUILD_DIR}}/mac.o
    ar rcs {{MAC_STATIC}} {{BUILD_DIR}}/mac.o
    rm {{BUILD_DIR}}/mac.o
    echo "✅ Fertig: {{MAC_STATIC}}"

# --- 3. Windows ---
@win-static: prepare
	echo "🪟 Baue Windows Static (.a) mit Clang..."
	{{CC}} -O3 -c {{SRC}} -o {{BUILD_DIR}}/win.o
	ar rcs {{WIN_STATIC}} {{BUILD_DIR}}/win.o
	rm {{BUILD_DIR}}/win.o
	echo "✅ Fertig: {{WIN_STATIC}}"

# --- 4. Linux (Wayland/X11) ---
@linux-static: prepare
    echo "🐧 Baue Linux Static (.a)..."
    {{CC}} -O3 -c {{SRC}} -o {{BUILD_DIR}}/linux.o
    ar rcs {{LINUX_STATIC}} {{BUILD_DIR}}/linux.o
    rm {{BUILD_DIR}}/linux.o
    echo "✅ Fertig: {{LINUX_STATIC}}"

# --- Alles bauen ---
@all: wasm mac-static win-static linux-static

# --- Build all native apps (Linux, Windows, macOS) ---
@native: linux-vulkan windows-vulkan macos-metal
    echo ""
    echo "  ✅ All native apps built successfully!"
    echo ""
    echo "  📦 Build outputs:"
    echo "    • Linux   (x86_64): Linux/build/bin/comboom_punkt_sucht_wallpaper"
    echo "    • Windows (x86_64): Windows/build/bin/comboom_punkt_sucht_wallpaper.exe"
    echo "    • macOS   (ARM64):  MacOS/build/Release/comboom.sucht Live Wallpaper.app"
    echo ""

# --- Rebuild all libraries + macOS app ---
@rebuild: clean all
    echo ""
    echo "  ✨ Rebuild complete!"
    echo ""

# --- Build all Linux variants (x86_64, ARM64) ---
@linux-all: linux-vulkan linux-vulkan-arm
    echo ""
    echo "  ✅ All Linux variants built!"
    echo ""

# --- Build all Windows variants (x86_64, ARM64) ---
@windows-all: windows-vulkan windows-vulkan-arm
    echo ""
    echo "  ✅ All Windows variants built!"
    echo ""

# --- Build all architectures (recommended for CI/release) ---
@all-platforms: linux-all windows-all macos-metal
    echo ""
    echo "  ✅ All platforms & architectures built!"
    echo ""
    echo "  📦 Available artifacts:"
    echo "    Linux:"
    echo "      • Linux/build/bin/comboom_punkt_sucht_wallpaper (x86_64)"
    echo "      • Linux/build-arm/bin/comboom_punkt_sucht_wallpaper (ARM64)"
    echo "    Windows:"
    echo "      • Windows/build/bin/comboom_punkt_sucht_wallpaper.exe (x86_64)"
    echo "      • Windows/build-arm/bin/comboom_punkt_sucht_wallpaper.exe (ARM64)"
    echo "    macOS:"
    echo "      • MacOS/build/Release/comboom.sucht Live Wallpaper.app (ARM64)"
    echo ""

# --- Linux Vulkan App ---
@linux-vulkan: linux-static
    echo "🐧 Baue Linux Vulkan App (X11/Wayland) mit Clang 18..."
    cd Linux && \
    cmake -B build \
      {{TOOLCHAIN}} \
      -DCMAKE_C_COMPILER={{CC}} \
      -DCMAKE_CXX_COMPILER={{CXX}} \
      -DCMAKE_BUILD_TYPE=Release \
      -DGLFW_USE_WAYLAND=ON \
      -DGLFW_USE_X11=ON && \
    cmake --build build --config Release --parallel
    echo "✅ Fertig: Linux/build/bin/comboom_punkt_sucht_wallpaper"

# --- Windows Vulkan App ---
@windows-vulkan: win-static
    echo "🪟 Baue Windows Vulkan App mit Clang..."
    cd Windows && \
    cmake -B build \
      {{TOOLCHAIN}} \
      -G Ninja \
      -DCMAKE_C_COMPILER={{CC}} \
      -DCMAKE_CXX_COMPILER={{CXX}} \
      -DCMAKE_BUILD_TYPE=Release && \
    cmake --build build --config Release --parallel
    echo "✅ Fertig: Windows/build/bin/comboom_punkt_sucht_wallpaper.exe"

# --- macOS Metal App (via Xcode) ---
@macos-metal: mac-static
    echo "🍎 Baue macOS Metal App..."
    cd MacOS && \
    xcodebuild -project "comboom.sucht Live Wallpaper.xcodeproj" \
               -scheme "comboom.sucht Live Wallpaper" \
               -configuration Release
    echo "✅ Fertig: MacOS/build/Release/comboom.sucht Live Wallpaper.app"

# --- Linux ARM64 Vulkan App (cross-compile) ---
@linux-vulkan-arm: linux-static
    echo "🐧 Baue Linux Vulkan App (ARM64 cross-compile)..."
    cd Linux && \
    cmake -B build-arm \
      {{TOOLCHAIN}} \
      -DCMAKE_C_COMPILER={{CC}} \
      -DCMAKE_CXX_COMPILER={{CXX}} \
      -DCMAKE_SYSTEM_NAME=Linux \
      -DCMAKE_SYSTEM_PROCESSOR=aarch64 \
      -DCMAKE_BUILD_TYPE=Release \
      -DGLFW_USE_WAYLAND=ON \
      -DGLFW_USE_X11=ON && \
    cmake --build build-arm --config Release --parallel
    echo "✅ Fertig: Linux/build-arm/bin/comboom_punkt_sucht_wallpaper (ARM64)"

# --- Windows ARM64 Vulkan App (cross-compile) ---
@windows-vulkan-arm: win-static
    echo "🪟 Baue Windows Vulkan App (ARM64 cross-compile) mit Clang..."
    cd Windows && \
    cmake -B build-arm \
      {{TOOLCHAIN}} \
      -G Ninja \
      -DCMAKE_C_COMPILER={{CC}} \
      -DCMAKE_CXX_COMPILER={{CXX}} \
      -DCMAKE_SYSTEM_NAME=Windows \
      -DCMAKE_SYSTEM_PROCESSOR=ARM64 \
      -DCMAKE_BUILD_TYPE=Release && \
    cmake --build build-arm --config Release --parallel
    echo "✅ Fertig: Windows/build-arm/bin/comboom_punkt_sucht_wallpaper.exe (ARM64)"

# --- Aufräumen ---
@clean:
    echo "🧹 Deleting build artifacts..."
    rm -rf {{BUILD_DIR}}
    rm -rf Linux/build Linux/build-arm
    rm -rf Windows/build Windows/build-arm
    rm -rf MacOS/build
    echo "✨ Clean!"

# --- Test/Run Linux app (development) ---
@run-linux: linux-vulkan
    echo ""
    echo "  🎮 Running Linux Vulkan app..."
    ./Linux/build/bin/comboom_punkt_sucht_wallpaper --h1 "Test" --h2 "App"

# --- Test/Run Windows app (if on Windows) ---
@run-windows: windows-vulkan
    echo ""
    echo "  🎮 Running Windows Vulkan app..."
    .\Windows\build\bin\comboom_punkt_sucht_wallpaper.exe --h1 "Test" --h2 "App"

# --- Open macOS app ---
@run-macos: macos-metal
    echo ""
    echo "  🎮 Opening macOS Metal app..."
    open "MacOS/build/Release/comboom.sucht Live Wallpaper.app"

# --- Show status/info ---
@info:
    @echo ""
    @echo "  ℹ️  Project Information"
    @echo "  ━━━━━━━━━━━━━━━━━━━━━"
    @echo ""
    @echo "  Project: comboom. sucht Native Wallpaper"
    @echo "  Version: 1.0.5"
    @echo "  Compiler: Clang 18 (primary), Apple Clang (macOS)"
    @echo "  C++ Standard: C++23"
    @echo "  Build System: CMake 3.20+"
    @echo ""
    @echo "  Graphics:"
    @echo "    • Linux/Windows: Vulkan 1.2+"
    @echo "    • macOS: Metal"
    @echo ""
    @echo "  Platforms:"
    @echo "    • Linux: X11 & Wayland"
    @echo "    • Windows: Win32"
    @echo "    • macOS: AppKit"
    @echo ""
    @echo "  Quick start:"
    @echo "    just linux-vulkan      # Build Linux app"
    @echo "    just windows-vulkan    # Build Windows app"
    @echo "    just macos-metal       # Build macOS app"
    @echo "    just all-platforms     # Build everything"
    @echo ""

# --- Shorthand aliases for convenience ---
alias l := linux-vulkan
alias w := windows-vulkan
alias m := macos-metal
alias b := native
alias ba := all-platforms
alias c := clean

# --- Convenience recipes (matches package.json scripts style) ---
@libs_linux:
    @just linux-static

@libs_windows:
    @just win-static

@libs_macos:
    @just mac-static

@libs_wasm:
    @just wasm

@libs_all:
    @just all
