# 🎨 comboom. sucht Native Wallpaper

> **Cross-platform animated particle wallpaper engine** with native apps for Linux, Windows, and macOS using modern graphics APIs.

![Build Status](https://img.shields.io/github/actions/workflow/status/comboom/comboom-punkt-sucht/release-native-apps.yml?style=flat-square)
![License](https://img.shields.io/badge/license-MIT-blue?style=flat-square)
![C++ Standard](https://img.shields.io/badge/C%2B%2B-23-brightgreen?style=flat-square)
![Platforms](https://img.shields.io/badge/platforms-Linux%20%7C%20Windows%20%7C%20macOS-blueviolet?style=flat-square)

---

## ✨ Features

- 🎯 **Particle Physics Engine**: Real-time simulation of 100+ interactive particles
- 🖱️ **Mouse Tracking**: Particles respond to cursor movement with smooth damping
- 📝 **Custom Text Overlay**: Display headers and subtitles with dynamic font sizing
- 🔄 **Cross-Platform**: Linux (X11/Wayland), Windows (Win32), macOS (Metal)
- 🎨 **Native Rendering**:
  - **Vulkan 1.2+** on Linux & Windows
  - **Metal** on macOS (Apple Silicon optimized)
- 🌍 **Multi-Architecture**: x86_64, ARM64 (Raspberry Pi, Apple Silicon, Snapdragon boards)
- 🔧 **System Integration**:
  - Linux: AppIndicator3 system tray
  - Windows: Windows taskbar tray
  - macOS: Native menu bar
- ⚡ **Performance**: Optimized for low latency, high FPS (60+)
- 🧹 **Zero Dependencies**: Minimal external libraries (GLFW + graphics SDK only)

---

## 🚀 Quick Start

### Linux (X11 & Wayland)

```bash
# Download latest release
tar -xzf comboom-sucht-linux-x86_64.tar.gz

# Run with custom text
./comboom_punkt_sucht_wallpaper --h1 "Hello" --h2 "Wallpaper"
```

### Windows

```cmd
# Download latest release
tar -xf comboom-sucht-windows-x86_64.zip

# Run
comboom_punkt_sucht_wallpaper.exe --h1 "Hello" --h2 "Wallpaper"
```

### macOS

```bash
# Download latest release
unzip comboom-sucht-macos-arm64.zip

# Launch
open "comboom.sucht Live Wallpaper.app"
```

---

## 📦 Installation

### From GitHub Releases

Download pre-built binaries from the [latest release](../../releases/latest):

| Platform   | Arch   | Format  | Size  |
| ---------- | ------ | ------- | ----- |
| 🐧 Linux   | x86_64 | .tar.gz | ~15MB |
| 🐧 Linux   | ARM64  | .tar.gz | ~15MB |
| 🪟 Windows | x86_64 | .zip    | ~20MB |
| 🪟 Windows | ARM64  | .zip    | ~20MB |
| 🍎 macOS   | ARM64  | .zip    | ~50MB |

### Build from Source

See [BUILD.md](BUILD.md) for detailed instructions.

```bash
# Clone repository
git clone https://github.com/comboom/comboom-punkt-sucht.git
cd comboom-punkt-sucht

# Build native apps
just all-platforms

# Artifacts in:
# - Linux/build/bin/comboom_punkt_sucht_wallpaper (x86_64)
# - Linux/build-arm/bin/comboom_punkt_sucht_wallpaper (ARM64)
# - Windows/build/bin/comboom_punkt_sucht_wallpaper.exe (x86_64)
# - Windows/build-arm/bin/comboom_punkt_sucht_wallpaper.exe (ARM64)
# - MacOS/build/Release/comboom.sucht Live Wallpaper.app (ARM64)
```

---

## 🛠️ Tech Stack

| Component                          | Technology                    | Details                               |
| ---------------------------------- | ----------------------------- | ------------------------------------- |
| **Language**                 | C++23                         | Modern, efficient, cross-platform     |
| **Compiler**                 | Clang 18                      | Full C++23 support, optimizations     |
| **Build System**             | CMake 3.20+                   | Portable, parallel builds             |
| **Task Runner**              | Justfile                      | Simple commands:`just linux-vulkan` |
| **Graphics (Linux/Windows)** | Vulkan 1.2+                   | High-performance rendering            |
| **Graphics (macOS)**         | Metal 3+                      | Native Apple GPU optimization         |
| **Window Management**        | GLFW 3.4+                     | Auto-detection: X11/Wayland/Win32     |
| **Memory Management**        | VMA + RAII                    | Safe GPU/CPU memory handling          |
| **System Tray**              | AppIndicator3/Shell API/Cocoa | Native platform integration           |

---

## 💻 System Requirements

### Linux

- **OS**: Ubuntu 22.04+, Debian 12+, Fedora 36+, or similar
- **GPU**: Vulkan-capable (Intel Gen 7+, AMD GCN, NVIDIA Kepler+)
- **CPU**: x86_64 or ARM64
- **RAM**: 256 MB free
- **Display**: X11 or Wayland session

### Windows

- **OS**: Windows 10 or later
- **GPU**: Vulkan-capable (Intel, AMD, NVIDIA)
- **CPU**: x86_64 or ARM64
- **RAM**: 256 MB free
- **Display**: Standard desktop or laptop

### macOS

- **OS**: macOS 12 (Monterey) or later
- **CPU**: Apple Silicon (M1, M2, M3, etc.)
- **RAM**: 256 MB free
- **GPU**: Metal-capable (all Apple Silicon have Metal)

---

## 🎮 Usage

### Command-Line Arguments

```bash
# Custom header text
--h1 "Your Title"

# Custom subtitle text
--h2 "Your Subtitle"

# Show help
--help

# Example
./comboom_punkt_sucht_wallpaper --h1 "The Universe" --h2 "Made of Particles"
```

### System Tray Integration

- **Linux**: Click tray icon → Settings window to configure text
- **Windows**: Right-click tray icon → Open settings
- **macOS**: Click menu bar icon → Open settings

---

## 📊 Architecture

### High-Level Overview

```
┌─────────────────────────────────────────┐
│         User Application Layer          │
│  (GLFW Window + Event Loop + Input)     │
└──────────────┬──────────────────────────┘
               │
┌──────────────v──────────────────────────┐
│      Rendering Layer (GPU)              │
│  • Vulkan (Linux/Windows)               │
│  • Metal (macOS)                        │
│  • Particle rendering pipeline          │
└──────────────┬──────────────────────────┘
               │
┌──────────────v──────────────────────────┐
│   C++ Engine Wrapper (RAII)             │
│  • Particle physics simulation          │
│  • State management                     │
│  • System tray integration              │
└──────────────┬──────────────────────────┘
               │
┌──────────────v──────────────────────────┐
│    C Core Engine (Platform-Agnostic)    │
│  • Pure particle system                 │
│  • Physics calculations                 │
│  • No external dependencies             │
└─────────────────────────────────────────┘
```

### Build Pipeline

```
Source Code (C++23)
    ↓
Clang 18 Compiler
    ↓
[Linux/Windows]          [macOS]
Vulkan Renderer          Metal Renderer
    ↓                    ↓
Static Binary            App Bundle
```

---

## 🔨 Development

### Building Locally

**Linux:**

```bash
just linux-vulkan          # Main binary (x86_64)
just linux-vulkan-arm      # ARM64 cross-compile
just run-linux             # Test drive
```

**Windows:**

```bash
just windows-vulkan        # Main binary (x86_64)
just windows-vulkan-arm    # ARM64 cross-compile
just run-windows           # Test drive
```

**macOS:**

```bash
just macos-metal           # Metal app
just run-macos             # Launch
```

### Build All Targets

```bash
just all-platforms         # Build 5 architectures for CI/release
```

### Project Structure

```
.
├── core/                      # C core engine (platform-agnostic)
│   └── cbps_wallpaper_engine_core.c
├── Linux/                     # Linux/Vulkan app
│   ├── CMakeLists.txt
│   └── src/
│       ├── main.cpp
│       ├── wallpaper_engine.hpp/cpp
│       ├── vulkan_renderer.hpp/cpp
│       ├── window_manager.hpp/cpp
│       ├── system_tray.hpp
│       ├── linux/system_tray_linux.cpp
│       └── shaders/
├── Windows/                   # Windows/Vulkan app
│   ├── CMakeLists.txt
│   └── src/
├── MacOS/                     # macOS/Metal app (Xcode)
│   └── comboom.sucht Live Wallpaper.xcodeproj
├── WASM/                      # Web version (SvelteKit + WebGL)
├── .github/workflows/         # CI/CD automation
├── Justfile                   # Build tasks
├── BUILD.md                   # Build instructions
├── JUSTFILE.md               # Justfile reference
└── RELEASE.md                # Release process
```

---

## 🚢 Release Process

### Automated CI/CD

- **Trigger**: Push a git tag (e.g., `v1.0.6`)
- **Build**: GitHub Actions compiles all 5 architectures in parallel
- **Upload**: Artifacts auto-upload to GitHub Release page
- **Notes**: Release notes auto-generated from commit history

See [RELEASE.md](RELEASE.md) for detailed release instructions.

---

## 📈 Performance

### Benchmarks (1080p, 100 particles)

| Platform | GPU                     | FPS  | GPU Load |
| -------- | ----------------------- | ---- | -------- |
| Linux    | NVIDIA RTX 3060         | 144+ | <5%      |
| Linux    | Intel Iris Xe           | 60+  | <10%     |
| Linux    | ARM Mali-G78            | 60   | ~15%     |
| Windows  | NVIDIA GeForce GTX 1660 | 144+ | <5%      |
| macOS    | M3 Max                  | 120  | <10%     |

*Lower GPU load = better battery life on laptops*

---

## 🐛 Troubleshooting

### Linux - Vulkan not found

```bash
sudo apt install vulkan-tools libvulkan-dev libvulkan1
```

### Windows - Missing Visual C++ runtime

Download: https://support.microsoft.com/en-us/help/2977003/

### macOS - App won't launch

```bash
# Remove quarantine attribute
xattr -d com.apple.quarantine "comboom.sucht Live Wallpaper.app"
```

### Build fails on ARM64 cross-compile

```bash
sudo apt install gcc-aarch64-linux-gnu binutils-aarch64-linux-gnu
```

See [BUILD.md](BUILD.md) for more help.

---

## 🤝 Contributing

Contributions welcome! Here's how:

1. **Fork** the repository
2. **Create a branch**: `git checkout -b feature/my-feature`
3. **Commit changes**: `git commit -am "Add my feature"`
4. **Push**: `git push origin feature/my-feature`
5. **Open PR** against `main`

### Guidelines

- Follow C++23 best practices
- Add comments for complex logic
- Test on Linux, Windows, and macOS if possible
- Update docs if adding features

See [CONTRIBUTING.md](CONTRIBUTING.md) for detailed guidelines.

---

## 📄 License

This project is licensed under the **MIT License** - see [LICENSE](LICENSE) for details.

```
MIT License

Copyright (c) 2024 comboom. sucht

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
```

---

## 🎯 Roadmap

### Current (v1.0.5)

- ✅ Core particle physics engine
- ✅ Linux Vulkan app (X11 & Wayland)
- ✅ Windows Vulkan app
- ✅ macOS Metal app
- ✅ System tray integration
- ✅ Multi-architecture support (x86_64, ARM64)

---



## 📞 Contact & Support

- **Issues**: [GitHub Issues](../../issues)
- **Discussions**: [GitHub Discussions](../../discussions)
- **Email**: support@comboom.sucht

---

## 🙏 Acknowledgments

- **GLFW**: Cross-platform window management
- **Vulkan**: Modern graphics API
- **vk-bootstrap**: Vulkan boilerplate
- **Vulkan Memory Allocator**: GPU memory management
- **CMake**: Build system
- **Clang/LLVM**: Compiler infrastructure

---

## 📊 Stats

```
Lines of Code:     ~5,000 (C++ engine + apps)
Supported Archs:   5 (Linux x86_64/ARM64, Windows x86_64/ARM64, macOS ARM64)
Particle Count:    100+ (configurable)
Build Time:        ~2 minutes (full rebuild)
Binary Size:       ~15-20 MB
```

---

<div align="center">

### Made with ❤️ using modern C++23 graphics APIs

**[⬆ back to top](#comboom-sucht-native-wallpaper)**

</div>
