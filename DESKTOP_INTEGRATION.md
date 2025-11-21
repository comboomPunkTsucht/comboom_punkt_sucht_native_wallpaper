# Desktop Wallpaper Integration

Die Anwendung läuft **automatisch** als Desktop-Wallpaper auf allen unterstützten Plattformen.

## ✅ Implementierter Status

### macOS ✅

- Fenster-Level wird unter Desktop-Icons gesetzt
- Fenster ist nicht bewegbar (`setMovable: false`)
- Sichtbar auf allen Spaces
- **Funktioniert out-of-the-box**

### Windows ✅

- Integration mit WorkerW/Progman für echten Desktop-Hintergrund
- Fenster wird als Child des Desktop-Windows gesetzt
- Füllt automatisch den Bildschirm
- **Funktioniert out-of-the-box**

### Linux/BSD ⚠️ (Teilweise implementiert)

- X11 Code ist vorbereitet in `desktop_wallpaper.hpp`
- **Benötigt noch**: GLFW/X11 Handle-Zugriff
- Setzt `_NET_WM_WINDOW_TYPE_DESKTOP` und Window-States
- **Alternative**: Verwende `xwinwrap` (siehe unten)

## 🚀 Verwendung

### Normale Verwendung (macOS/Windows)

```bash
./build/cbps-lwp
```

Die App startet automatisch als Wallpaper!

### Linux mit xwinwrap (empfohlen bis X11-Integration fertig)

```bash
xwinwrap -g 1920x1080 -ov -ni -s -nf -b -un -argb -fdt -- \
  ./build/cbps-lwp
```

## 🔧 Technische Details

### Dateien

- **`src/desktop_wallpaper.hpp`**: Platform-spezifische Wallpaper-Integration
- **`src/main.cpp`**: Automatischer Aufruf nach `InitWindow()`

### Code-Flow

1. `InitWindow()` wird aufgerufen
2. Platform-Check (#ifdef)
3. Native Window-Handle holen
4. `DesktopWallpaper::SetAsWallpaper()` aufrufen
   - macOS: NSWindow Objective-C Calls
   - Windows: Win32 WorkerW/Progman
   - Linux: X11 Atom Properties (TODO)

## 📝 Nächste Schritte für Linux

Für vollständige Linux-Integration ohne xwinwrap:

```cpp
// In main.cpp, nach InitWindow():
#if defined(__linux__) || defined(__FreeBSD__)
    #include <GLFW/glfw3.h>
    #include <GLFW/glfw3native.h>

    GLFWwindow* glfwWin = /* Get from raylib somehow */;
    Display* display = glfwGetX11Display();
    Window window = glfwGetX11Window(glfwWin);
    DesktopWallpaper::SetAsWallpaper(display, window);
#endif
```

**Problem**: Raylib versteckt GLFW-Handles. Mögliche Lösungen:

1. Raylib-Source modifizieren
2. `xwinwrap` verwenden (aktuell empfohlen)
3. Direkt X11 ohne Raylib nutzen
