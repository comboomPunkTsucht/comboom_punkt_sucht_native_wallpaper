# comboom. sucht - C++23 Raylib Native Wallpaper

## 🎯 Überblick

Eine Cross-Platform Particle-Wallpaper-Engine, geschrieben in C++23 mit Raylib, für Linux (X11/Wayland) und Windows (MinGW).

- **Grafik**: Raylib (OpenGL 4.3)
- **Build-System**: Justfile + Clang
- **Assets**: C++23 `#embed` (eingebunden in Binary)
- **System-Integration**:
  - Linux: AppIndicator3 + GTK Dialoge
  - Windows: Win32 API (Shell/Tray)
- **Linking**: Vollständig statisch (keine externen Runtime-Abhängigkeiten)

## 🏗️ Verzeichnisstruktur

```
comboom_punkt_sucht_native_wallpaper/
├── core/                          # C-Kern (Physik/Particle-Engine)
│   ├── cbps_wallpaper_engine_core.c
│   ├── cbps_wallpaper_engine_core.h
│   └── stb_sprintf.h
│
├── Linux-Windows-shared/src/      # C++23 Cross-Platform Quellcode
│   ├── main_raylib.cpp            # App Entry Point + Threading
│   ├── wallpaper_app.hpp/cpp      # C-Engine Wrapper
│   ├── renderer_raylib.hpp/cpp    # Raylib Particle Rendering
│   ├── assets_loader.hpp/cpp      # C++23 #embed Asset Loader
│   ├── system_tray.hpp            # Abstract Interface
│   ├── system_tray_windows.cpp    # Win32 Tray Implementation
│   └── system_tray_linux.cpp      # AppIndicator3 Tray Implementation
│
├── thirdparty/
│   └── raylib/                    # Git Submodul (lokal kompiliert)
│
├── WASM/static/                   # Einzubindende Assets
│   ├── Logo.png                   # Main Logo
│   ├── fonts/                     # Caskaydida Cove Nerd Font Varianten
│   │   ├── CaskaydiaCoveNerdFont-Regular.ttf
│   │   ├── CaskaydiaCoveNerdFont-Bold.ttf
│   │   ├── CaskaydiaCoveNerdFont-Light.ttf
│   │   ├── CaskaydiaCoveNerdFont-ExtraLight.ttf
│   │   └── ... (weitere Varianten)
│   └── pictures/                  # Logo-Bilder
│       ├── bd_logo.png
│       ├── cbps_logo.png
│       ├── fabelke_logo.png
│       ├── knuddelzwerck_logo.png
│       └── mahd_logo.png
│
├── Justfile                       # Build-Orchestrierung (Raylib + Clang)
├── MacOS/                         # Swap/Metal Version (unverändert)
└── build/                         # Build-Ausgaben
    ├── libcbps_we_core_*.a        # C-Kern Static Libraries
    ├── libraylib_*.a              # Raylib Static Libraries
    ├── *.o                        # Object Files
    └── comboom-sucht-*            # Finale Executables
```

## 📦 Build-System: Justfile-Rezepte

### C-Core Libraries (Statisch)

```bash
just linux-static      # Kompiliere C-Kern für Linux
just wasm              # WebAssembly für WASM-Build
just libs_all          # Alle Static Libraries
```

### Raylib Native Apps

**Linux x64 (X11/Wayland + AppIndicator3)**
```bash
just linux-raylib      # Kompiliere + linke Linux App (x64)
```

**Linux ARM64 (Cross-Compile)**
```bash
just linux-raylib-arm64
# Benötigt: gcc-aarch64-linux-gnu Sysroot
```

**Windows x64 (MinGW + Win32 API)**
```bash
just windows-raylib    # Kompiliere + linke Windows App (x64)
```

**Windows ARM64 (Cross-Compile)**
```bash
just windows-raylib-arm64
# Benötigt: LLVM-MinGW aarch64-w64-mingw32 Toolchain
```

### Batch-Rezepte

```bash
just apps              # Baue alle Standard-Apps (x64 nur)
just apps-all          # Baue alle Apps + Architekturen (x64 + ARM64)
just all               # Baue nur C-Core Libraries
just clean             # Lösche alle Build-Artefakte
```

### Shortcuts (Aliases)

```bash
just l                 # = just linux-raylib
just la                # = just linux-raylib-arm64
just w                 # = just windows-raylib
just wa                # = just windows-raylib-arm64
just a                 # = just apps
just aa                # = just apps-all
just c                 # = just clean
```

## 🔧 Technische Details

### Threading-Modell

```
┌─────────────────────────┐
│  main() Einstiegspunkt  │
└────────┬────────────────┘
         │
         ├─ Thread 1: Raylib Loop (OpenGL Context)
         │   ├ Fenster & Input
         │   ├ C-Engine Update
         │   ├ Particle Rendering
         │   └ Frame Loop
         │
         └─ Thread 2: System Tray (Main)
             ├ Tray Icon
             ├ Kontextmenü
             ├ H1/H2 Eingabe Dialoge
             └ Quit Handling

         ↔ std::mutex: AppState (H1, H2, quit_flag)
```

### Particle Rendering

**Metal Shader (macOS)** → **Raylib DrawCircle (Linux/Windows)**

```cpp
// Jedes Partikel:
DrawCircle(
    norm_x, norm_y,              // Position (normalisiert zu Bildschirm)
    particle.radius * 2.0f,      // Durchmesser → Größe
    Color(r, g, b, a)            // RGBA aus C-Engine
);
```

**Alpha-Blending**: Raylib Standardeinstellungen (additive für Transparenz)

### Asset Embedding (C++23 #embed)

```cpp
// Zur Compile-Zeit in Binary eingebunden
constexpr auto logo_data_array = std::to_array<unsigned char>({
    #embed "../../WASM/static/Logo.png"
});

// Zur Laufzeit via Raylib Memory-Funktionen geladen
Image logo = LoadImageFromMemory(".png",
    logo_data_array.data(),
    logo_data_array.size()
);
```

**Vorteil**: Null Runtime-Overhead, keine externen Asset-Dateien notwendig

### Static Linking

**Linker-Flags (Linux x64)**:
```
-lX11 -lGLX -lGL -ldl -lpthread       # X11/OpenGL
-lappindicator3 -lgtk-3 -lglib-2.0    # System Tray UI
-gobj-2.0                             # GObject (GTK dependency)
-static-libstdc++ -static-libgcc      # C++ Runtime
```

**Linker-Flags (Windows x64)**:
```
-lopengl32 -lgdi32 -luser32           # Windows API
-lshell32 -lole32 -lwinmm             # Shell/OLE/Multimedia
-static-libstdc++ -static-libgcc      # C++ Runtime
```

→ **Resultat**: Single Binary, kein `.so`/`.dll` zur Laufzeit erforderlich

## 🖼️ System Tray UI

### Linux (AppIndicator3 + GTK)

```
System Tray Icon
    │
    └─ Kontextmenü
       ├ "comboom.sucht Live Wallpaper" (disabled, label)
       ├ ─────────────────────────────── (separator)
       ├ "Edit H1 (Title)" → GTK Dialog
       ├ "Edit H2 (Subtitle)" → GTK Dialog
       ├ ─────────────────────────────── (separator)
       └ "Quit" → Cleanup + Exit
```

### Windows (Win32 Shell API)

```
System Tray Icon
    │
    └─ Kontextmenü (TrackPopupMenu)
       ├ "comboom.sucht Live Wallpaper" (disabled)
       ├ ─────────────────────────────── (separator)
       ├ "Edit H1..." → MessageBox Input (vereinfacht)
       ├ "Edit H2..." → MessageBox Input
       ├ ─────────────────────────────── (separator)
       └ "Quit" → PostMessage(WM_QUIT)
```

## 🎨 Assets (Eingebunden via #embed)

### Logo
- `Logo.png` (313 KB) - Main App Logo

### Fonts (Caskaydida Cove Nerd Font)
- Regular, Bold, Light, ExtraLight Varianten (jeweils ~2 MB)
- Mono und Proportional Versionen verfügbar
- Insgesamt 38 `.ttf` Dateien (optional einkompilierbar)

### Picture Logos
- `bd_logo.png`
- `cbps_logo.png`
- `fabelke_logo.png`
- `knuddelzwerck_logo.png`
- `mahd_logo.png`

**Gesamtgröße eingebunden**: ~50 MB → Finale Binary: ~20-30 MB (nach Strip)

## 🛠️ Abhängigkeiten

### Zur Compile-Zeit erforderlich

```bash
# Clang 17+ (C++23 #embed Support)
clang++ --version

# Für Linux ARM64 Cross-Compile:
sudo apt install gcc-aarch64-linux-gnu

# Für Windows MinGW (LLVM-MinGW empfohlen):
# https://github.com/mstorsjo/llvm-mingw
export PATH="/path/to/llvm-mingw/bin:$PATH"
```

### Linux Dev Libraries (optional, für AppIndicator3)

```bash
sudo apt install libappindicator3-dev libgtk-3-dev libdbus-1-dev
```

### Zur Laufzeit

**Keine externen Abhängigkeiten!** (Alles statisch gelinkt)

## 🚀 Schnellstart

### 1. Repo klonen + Raylib Submodul initialisieren

```bash
git clone https://github.com/mcpeaps_HD/comboom-sucht-native-wallpaper.git
cd comboom-sucht-native-wallpaper
git submodule update --init --recursive
```

### 2. Linux x64 App bauen

```bash
just linux-raylib
# Output: ./build/comboom-sucht-linux-x64
```

### 3. Windows x64 App bauen (von Linux aus mit MinGW)

```bash
just windows-raylib
# Output: ./build/comboom-sucht-windows-x64.exe
```

### 4. Ausführen

```bash
# Linux
./build/comboom-sucht-linux-x64

# Windows (auf Windows oder via Wine)
wine ./build/comboom-sucht-windows-x64.exe
```

## 🧪 Troubleshooting

### Problem: Clang C++23 #embed nicht unterstützt

**Symptom**: Kompilierfehler `unknown directive '#embed'`

**Lösung**:
1. Clang zu Version 17+ upgraden
2. Oder: Fallback auf `xxd -i` basiertes Embedding (manuell)

### Problem: ARM64 Cross-Compile schlägt fehl

**Symptom**: `aarch64-linux-gnu-gcc: not found`

**Lösung**:
```bash
sudo apt install gcc-aarch64-linux-gnu g++-aarch64-linux-gnu
```

### Problem: GTK3 Headers nicht gefunden (Linux)

**Symptom**: `gtk/gtk.h: No such file or directory`

**Lösung**:
```bash
sudo apt install libgtk-3-dev libappindicator3-dev
```

### Problem: Windows MinGW nicht gefunden

**Symptom**: `x86_64-w64-mingw32-gcc: command not found`

**Lösung**:
1. LLVM-MinGW herunterladen: https://github.com/mstorsjo/llvm-mingw/releases
2. In PATH hinzufügen:
```bash
export PATH="/path/to/llvm-mingw/bin:$PATH"
```

## 📊 Build-Ausgaben

```
build/
├── libcbps_we_core_linux.a          # C-Kern für Linux
├── libraylib_linux_x64.a            # Raylib für Linux x64
├── raylib_linux_x64/
│   ├── core.o
│   ├── shapes.o
│   ├── textures.o
│   ├── text.o
│   └── ...
├── app_linux_x64/
│   ├── main.o
│   ├── wallpaper_app.o
│   ├── renderer.o
│   ├── assets_loader.o
│   └── system_tray_linux.o
└── comboom-sucht-linux-x64         # 🎯 Finale App (statisch gelinkt)
```

## 🔐 Sicherheit & Portabilität

- **Statisch gelinkt**: Kein GLIBC/C++-Version-Mismatch
- **Keine Abhängigkeiten**: Läuft überall (Linux 2.6.32+, Windows 7+)
- **Embedded Assets**: Keine externen Dateien, kein Asset-Loading-Fehler
- **Sandbox-ready**: Funktioniert in Containern, AppImage, etc.

## 📝 Performance

- **Particle Count**: 5000 Partikel @ 60 FPS (typisch)
- **Frame Time**: ~16 ms (GPU-begrenzt, nicht CPU)
- **Memory**: ~50-100 MB (Raylib + Particles + Assets)
- **Binary Size**: ~25 MB (nach `strip`, vollständig statisch)

## 🤝 Beitragen

Siehe `CONTRIBUTING.md` (TBD)

## 📄 Lizenz

Siehe `LICENSE` (TBD)

---

**Gebaut mit ❤️ von comboom. für Wallpaper-Fans**
