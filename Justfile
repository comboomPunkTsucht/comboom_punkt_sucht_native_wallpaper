# --- Konfiguration ---
CC := "clang"
SRC := "core/cbps_wallpaper_engine_core.c"
BUILD_DIR := "build"

# --- Ausgabe-Dateien ---
WASM_OUT     := BUILD_DIR + "/cbps_we_core.wasm"
MAC_STATIC   := BUILD_DIR + "/libcbps_we_core_mac.a"
WIN_STATIC   := BUILD_DIR + "/libcbps_we_core_win.a"
LINUX_STATIC := BUILD_DIR + "/libcbps_we_core_linux.a"

# Standard-Task: Zeigt das Hilfe-Menü an
default:
    @echo "Verfügbare Befehle (Nur Statisch):"
    @echo "  just wasm         - Baue WebAssembly (.wasm)"
    @echo "  just mac-static   - Baue macOS Static Library (.a)"
    @echo "  just win-static   - Baue Windows Static Library (.a)"
    @echo "  just linux-static - Baue Linux Static Library (.a)"
    @echo "  just all          - Versucht alle 4 Targets zu bauen"
    @echo "  just clean        - Löscht den Build-Ordner"

@prepare:
    mkdir -p {{BUILD_DIR}}

# --- 1. WebAssembly ---
@wasm: prepare
    echo "🚀 Baue WebAssembly (.wasm)..."
    {{CC}} --target=wasm32-unknown-unknown -nostdlib -O3 -flto -Wl,--no-entry -Wl,--export-dynamic -o {{WASM_OUT}} {{SRC}}
    echo "✅ Fertig: {{WASM_OUT}}"

# --- 2. macOS ---
@mac-static: prepare
    echo "🍎 Baue macOS Static (.a)..."
    {{CC}} -O3 -c {{SRC}} -o {{BUILD_DIR}}/mac.o
    ar rcs {{MAC_STATIC}} {{BUILD_DIR}}/mac.o
    rm {{BUILD_DIR}}/mac.o
    echo "✅ Fertig: {{MAC_STATIC}}"

# --- 3. Windows ---
@win-static: prepare
    echo "🪟 Baue Windows Static (.a)..."
    # Kompiliere zur Objektdatei (.o) für Windows
    x86_64-w64-mingw32-clang -O3 -c {{SRC}} -o {{BUILD_DIR}}/win.o 2>/dev/null || \
    {{CC}} --target=x86_64-pc-windows-gnu -O3 -c {{SRC}} -o {{BUILD_DIR}}/win.o
    # Erstelle das statische Archiv
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

# --- Aufräumen ---
@clean:
    echo "🧹 Lösche {{BUILD_DIR}}..."
    rm -rf {{BUILD_DIR}}
    echo "✨ Sauber!"
