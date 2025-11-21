# comboom_punkt_sucht_native_wallpaper

Ein plattformübergreifendes Live-Wallpaper mit animierten Partikeln, verfügbar als native Desktop-Anwendung und als Web-Version.

> **ℹ️ Hinweis:** Dies ist die neue native C++ Implementierung des ursprünglichen [live-wallpaper](https://github.com/comboomPunkTsucht/live-wallpaper) Projekts mit WebAssembly-Unterstützung.

**Web-Version:** [live-wallpaper.comboompunktsucht.app](https://live-wallpaper.comboompunktsucht.app)

## 🌐 Web-Version

Die Anwendung läuft direkt im Browser ohne Installation!

### URL Query Parameters

Passe die Anzeige über URL-Parameter an:

- `?h1=<Text>` - Setzt den Haupttitel (Heading 1)
- `?h2=<Text>` - Setzt den Untertitel (Heading 2)
- `?width=<Zahl>` - Setzt die Canvas-Breite (optional)
- `?height=<Zahl>` - Setzt die Canvas-Höhe (optional)

**Beispiele:**

```
https://live-wallpaper.comboompunktsucht.app?h1=Willkommen&h2=Mein%20Projekt
https://live-wallpaper.comboompunktsucht.app?h1=mcpeaps_HD&h2=comboom.sucht
https://live-wallpaper.comboompunktsucht.app?h1=BlackDragon&h2=comboom.sucht&width=1920&height=1080
```

> **💡 Tipp:** Verwende den Untertitel `comboom.sucht` um das CBPS-Logo anzuzeigen!
>
> **🎨 Logo-Feature:** Bei Titeln wie `mcpeaps_HD`, `BlackDragon`, `knuddelzwerck` oder `fabelke` erscheint automatisch ein passendes Logo!

## 💻 Native Desktop-Anwendung

### Systemanforderungen

- C++23-kompatibler Compiler (GCC 13+, Clang 16+, oder MSVC 2022+)
- CMake 3.16 oder höher
- Raylib (wird automatisch gebaut)
- GLFW (Homebrew auf macOS empfohlen)

### Build-Prozess

Die Anwendung nutzt `nob` (ein Nobuild-Tool) für den Build-Prozess.

#### 1. Native Desktop-Build (Debug)

```bash
make all
```

Oder explizit:

```bash
make build_debug
./nob -b -d
```

#### 2. Native Desktop-Build (Release)

```bash
make build_rel
./nob -b
```

#### 3. Web-Build (WebAssembly)

```bash
make web
./nob -w
```

Der Web-Build erstellt automatisch:

- Emscripten SDK (falls nicht vorhanden)
- Raylib für WebAssembly
- Die fertige Web-App im `.web/` Verzeichnis

**Lokal testen:**

```bash
cd .web
python3 -m http.server 8000
```

Dann öffne `http://localhost:8000` im Browser.

#### 4. Aufräumen

```bash
make clean
```

### Command-line Flags (Native)

Die Desktop-Version unterstützt folgende Flags (via tsoding/flag.h):

- `-v` / `--version` - Zeigt die Versionsinformation
- `-h` / `--help` - Zeigt die Hilfe
- `--title <string>` - Setzt den Haupttitel (Standard: "Comboom Punkt Sucht Native Wallpaper")
- `--subtitle <string>` - Setzt den Untertitel
- `--width <int>` - Setzt die Fensterbreite (Standard: 1280)
- `--height <int>` - Setzt die Fensterhöhe (Standard: 720)

**Beispiel:**

```bash
./build/cbps-lwp --title "McPeAps_HD" --subtitle "comboom.sucht" --width 1920 --height 1080
```

## 🖥️ Desktop-Wallpaper Integration

> **⚠️ Status:** Die Desktop-Wallpaper-Integration ist derzeit experimentell.
>
> **🤝 Pull Requests erwünscht!** Beiträge zur Verbesserung der Desktop-Integration sind herzlich willkommen.

### macOS - Notlösung mit wallpaper-play

Die native Desktop-Integration funktioniert unter macOS derzeit nicht. Als **Notlösung** kann [wallpaper-play](https://github.com/nhiroyasu/wallpaper-play) verwendet werden, das die **Web-Version** lädt:

```bash
# 1. wallpaper-play installieren (folge der Anleitung im Repository)
# 2. Web-Version als Wallpaper laden
# wallpaper-play lädt: https://live-wallpaper.comboompunktsucht.app
```

**Vorteile:**

- ✅ Funktioniert sofort ohne native Kompilierung
- ✅ Nutzt die bereits deployte Web-Version
- ✅ Einfache URL-Parameter-Konfiguration

**Alternative (Normales Fenster):**

```bash
./build/cbps-lwp --width <screen_width> --height <screen_height>
```

### Windows - Lively Wallpapers

Für Windows kann [Lively Wallpapers](https://github.com/rocksdanister/lively) verwendet werden:

```bash
# 1. Lively Wallpapers installieren
# 2. Web-Version als Wallpaper hinzufügen:
#    URL: https://live-wallpaper.comboompunktsucht.app
```

**Native Code (nicht getestet):**

```bash
# WorkerW/Progman-Integration ist implementiert, aber nicht getestet
./build/cbps-lwp.exe
```

### Linux - xwinwrap

Für Linux kann `xwinwrap` verwendet werden:

```bash
xwinwrap -g 1920x1080 -ov -ni -s -nf -b -un -argb -fdt -- \
  ./build/cbps-lwp
```

**X11-Integration:** Code ist in `desktop_wallpaper.hpp` vorbereitet, benötigt aber noch GLFW/X11 Handle-Zugriff.

## 🚀 Web-Deployment

Die Web-Version wird nach dem Build automatisch im `.web/` Verzeichnis erstellt.

### Deployment-Dateien

```
.web/
├── index.html          # Haupt-HTML-Datei
├── index.js            # Kompiliertes WebAssembly (JS-Wrapper)
├── index.wasm          # WebAssembly Binary
├── index.data          # Preloaded Assets (Fonts, Bilder, Shaders)
└── assets/             # Asset-Verzeichnis (eingebettet in index.data)
```

### Deployment auf Server

1. **Build erstellen:**

   ```bash
   make web
   ```

2. **Verzeichnis hochladen:**
   Lade den kompletten Inhalt von `.web/` auf deinen Webserver hoch.
3. **Server-Konfiguration:**
   Stelle sicher, dass dein Webserver die richtigen MIME-Types setzt:

   ```nginx
   # Nginx Beispiel
   location ~ \.wasm$ {
       add_header Content-Type application/wasm;
   }
   ```

4. **CORS beachten:**
   Falls Assets von anderer Domain geladen werden:

   ```nginx
   add_header Access-Control-Allow-Origin *;
   ```

### Cloudflare Workers (Empfohlen)

Cloudflare Workers ist die moderne Methode für Cloudflare-Deployment:

```bash
# Nach dem Web-Build deployen
npx wrangler deploy --assets=./.web/ --compatibility-date 2025-11-21
```

**Vorteile:**

- ✅ Globales CDN
- ✅ Schnelles Deployment
- ✅ Keine Server-Konfiguration nötig

### Weitere Static-Hosting-Optionen

Die Web-Version funktioniert auf jedem Static-File-Hosting:

- GitHub Pages
- Netlify
- Vercel
- Cloudflare Pages
- Firebase Hosting
- S3 + CloudFront

Einfach den Inhalt von `.web/` hochladen!

## 📁 Projektstruktur

```
.
├── src/                          # C++ Quellcode
│   ├── main.cpp                  # Hauptprogramm
│   ├── constants.hpp             # Konstanten
│   ├── desktop_wallpaper.hpp     # Desktop-Integration
│   └── nord.hpp                  # Nord-Farbschema
├── web_src/                      # Web-spezifische Dateien
│   └── shell.html                # Emscripten HTML-Template
├── assets/                       # Assets (Fonts, Bilder, Shaders)
├── thirdparty/                   # Externe Bibliotheken
│   ├── raylib/                   # Raylib (Graphics)
│   └── flag.h/                   # Command-line Parsing
├── build/                        # Native-Build-Ausgabe (generiert)
├── nob.c                         # Build-Tool
├── Makefile                      # Build-Automation
└── .web/                         # Web-Build-Ausgabe (generiert)
```

## 🔧 Technische Details

### Build-System

Das Projekt verwendet `nob` (Nobuild) als Build-Tool:

- **Native**: Nutzt `g++`/`clang++` mit C++23
- **Web**: Nutzt Emscripten (`emcc`) für WebAssembly
- **Raylib**: Wird automatisch für die jeweilige Plattform gebaut

### Particle-System

- Nord-Farbschema (NORD11 rot, NORD14 grün)
- Magnetische Anziehung zur Maus
- Konstante Drift-Bewegung
- Automatisches Respawning außerhalb der Grenzen

### Shaders (Web + Native)

- **Particle Shader**: Optimiertes Rendering der Partikel
- **Gradient Shader**: Dekorative Linien mit Farbverläufen

## 🎨 Features

- ✅ C++23 Standard
- ✅ Raylib Graphics
- ✅ Nord-Farbschema
- ✅ Command-line Parsing (flag.h)
- ✅ Desktop-Wallpaper-Modus
- ✅ WebAssembly-Unterstützung
- ✅ Responsive Design
- ✅ URL-Query-Parameter (Web)
- ✅ Logo-Integration
- ✅ Hardware-beschleunigtes Rendering
- ✅ Cross-Platform (macOS, Windows, Linux, Web)

## 📄 Lizenz

Siehe LICENSE-Datei für Details.
