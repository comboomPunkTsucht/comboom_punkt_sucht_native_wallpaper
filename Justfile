# --- Konfiguration ---
CC := "clang-18"
CXX := "clang++-18"
SRC := "core/cbps_wallpaper_engine_core.c"
BUILD_DIR := "build"

# --- Ausgabe-Dateien ---
WASM_OUT     := BUILD_DIR + "/cbps_we_core.wasm"
MAC_STATIC   := BUILD_DIR + "/libcbps_we_core_mac.a"
WIN_STATIC   := BUILD_DIR + "/libcbps_we_core_win.a"
LINUX_STATIC := BUILD_DIR + "/libcbps_we_core_linux.a"

# Standard-Task: Zeigt das Hilfe-Menü an
default:
    @echo "=== C-Core Libraries (Statisch) ==="
    @echo "  just wasm         - Baue WebAssembly (.wasm)"
    @echo "  just mac-static   - Baue macOS Static Library (.a)"
    @echo "  just win-static   - Baue Windows Static Library (.a)"
    @echo "  just linux-static - Baue Linux Static Library (.a)"
    @echo "  just all          - Versucht alle 4 Targets zu bauen"
    @echo ""
    @echo "=== Native Apps (Vulkan) ==="
    @echo "  just linux-vulkan - Baue Linux Vulkan App (X11/Wayland)"
    @echo "  just windows-vulkan - Baue Windows Vulkan App"
    @echo "  just macos-metal  - Baue macOS Metal App (via Xcode)"
    @echo ""
    @echo "  just clean        - Löscht den Build-Ordner"

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
	clang-18 -O3 -c {{SRC}} -o {{BUILD_DIR}}/win.o
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

# --- Linux Vulkan App ---
@linux-vulkan: linux-static
    echo "🐧 Baue Linux Vulkan App (X11/Wayland) mit Clang 18..."
    cd Linux && \
    cmake -B build \
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
      -G Ninja \
      -DCMAKE_C_COMPILER=clang \
      -DCMAKE_CXX_COMPILER=clang++ \
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

# --- Aufräumen ---
@clean:
    echo "🧹 Lösche {{BUILD_DIR}}..."
    rm -rf {{BUILD_DIR}}
    echo "✨ Sauber!"
