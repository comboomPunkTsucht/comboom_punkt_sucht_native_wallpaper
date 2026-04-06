# Build Guide - Linux, Windows, macOS

## Quick Start

```bash
# Build C-core library first
just libs:linux    # or libs:windows, libs:macos

# Build native app
just linux-vulkan  # or windows-vulkan, macos-metal
```

---

## Prerequisites

### Common (All Platforms)
- **C++23 Compiler**: Clang 17+ (primary), GCC 13+ (fallback)
- **CMake**: 3.20 or later
- **Git**: For version control
- **just**: Task runner (optional, for convenience)

### Linux (Ubuntu/Debian 22.04+)
```bash
sudo apt-get update
sudo apt-get install -y \
    build-essential \
    cmake \
    ninja-build \
    pkg-config \
    just \
    clang-18 \
    clang++-18 \
    libomp-18-dev \
    lld-18 \
    libglfw3-dev \
    vulkan-tools \
    libvulkan-dev \
    libvulkan1 \
    glslang-tools \
    libappindicator3-dev \
    libx11-dev \
    libxrandr-dev \
    libxi-dev \
    libxinerama-dev \
    libxcursor-dev
```

#### Multi-architecture Support (Linux)

**ARM64 cross-compilation**:
```bash
sudo apt-get install -y \
    clang-18 \
    gcc-aarch64-linux-gnu \
    binutils-aarch64-linux-gnu \
    libvulkan-dev:arm64 \
    libx11-dev:arm64 \
    libxrandr-dev:arm64
```

**x86 (32-bit) support**:
```bash
sudo apt-get install -y \
    clang-18 \
    libomp-18-dev \
    libglfw3-dev:i386 \
    libvulkan-dev:i386
```

### Linux (Fedora/RHEL)
```bash
sudo dnf install -y \
    cmake \
    ninja-build \
    clang-tools-extra \
    glfw-devel \
    vulkan-headers \
    vulkan-loader-devel \
    glslang \
    libappindicator-devel
```

### Linux (Arch)
```bash
sudo pacman -S \
    cmake \
    ninja \
    clang \
    lld \
    openmp \
    glfw-x11 \
    vulkan-headers \
    vulkan-icd-loader \
    glslang \
    libappindicator-gtk3
```

### Windows (Clang/LLVM + Vulkan SDK)

Using Chocolatey:
```bash
choco install -y llvm cmake ninja vulkan-sdk
```

Or manually:
1. Download LLVM from https://github.com/llvm/llvm-project/releases
2. Download CMake from https://cmake.org/download/
3. Download Vulkan SDK from https://www.lunarg.com/vulkan-sdk/
4. Add all to system PATH

### macOS (Apple Clang + Metal)
```bash
# Xcode Command Line Tools
xcode-select --install

# Homebrew dependencies
brew install llvm cmake ninja vulkan-sdk glslang
```

---

## Architecture Support

| Platform | x86-64 | ARM64 | x86 | Compiler |
|----------|--------|-------|-----|----------|
| Linux    | ✅     | ✅    | ✅  | Clang 18 |
| Windows  | ✅     | ✅    | ✅  | LLVM     |
| macOS    | ❌     | ✅    | ❌  | Apple Clang|

---

## Building

### 1. Build C-Core Library First
```bash
# Linux (all architectures)
just libs:linux

# Windows (all architectures)
just libs:windows

# macOS
just libs:macos

# All platforms at once
just all
```

Outputs static libraries in `build/`:
- `libcbps_we_core_linux.a`
- `libcbps_we_core_win.a`
- `libcbps_we_core_mac.a`

### 2. Build Vulkan Apps

#### Linux (x86_64)
```bash
just linux-vulkan
# Output: Linux/build/bin/comboom_punkt_sucht_wallpaper
```

#### Linux (ARM64 cross-compilation)
```bash
cd Linux
cmake -B build \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_C_COMPILER=aarch64-linux-gnu-clang-18 \
  -DCMAKE_CXX_COMPILER=aarch64-linux-gnu-clang++-18 \
  -DCMAKE_SYSTEM_NAME=Linux \
  -DCMAKE_SYSTEM_PROCESSOR=aarch64 \
  -DGLFW_USE_WAYLAND=ON \
  -DGLFW_USE_X11=ON
cmake --build build --config Release
```

#### Linux (x86 32-bit)
```bash
cd Linux
cmake -B build \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_C_COMPILER=clang-18 \
  -DCMAKE_CXX_COMPILER=clang++-18 \
  -DCMAKE_C_FLAGS=-m32 \
  -DCMAKE_CXX_FLAGS=-m32 \
  -DGLFW_USE_WAYLAND=ON \
  -DGLFW_USE_X11=ON
cmake --build build --config Release
```

#### Windows (x86_64)
```bash
just windows-vulkan
# Output: Windows/build/bin/comboom_punkt_sucht_wallpaper.exe
```

#### Windows (ARM64 cross-compilation)
```bash
cd Windows
cmake -B build \
  -G Ninja \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_C_COMPILER=clang \
  -DCMAKE_CXX_COMPILER=clang++ \
  -DCMAKE_SYSTEM_NAME=Windows \
  -DCMAKE_SYSTEM_PROCESSOR=ARM64
cmake --build build --config Release
```

#### Windows (x86 32-bit)
```bash
cd Windows
cmake -B build \
  -G Ninja \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_C_COMPILER=clang \
  -DCMAKE_CXX_COMPILER=clang++ \
  -DCMAKE_C_FLAGS=-m32 \
  -DCMAKE_CXX_FLAGS=-m32
cmake --build build --config Release
```

#### macOS
```bash
just macos-metal
# Output: MacOS/build/Release/comboom.sucht Live Wallpaper.app
```

### 3. Run the Apps
```bash
# Linux
./Linux/build/bin/comboom_punkt_sucht_wallpaper --h1 "Hello" --h2 "World"

# Windows
Windows\build\bin\comboom_punkt_sucht_wallpaper.exe --h1 "Hello" --h2 "World"

# macOS
open "MacOS/build/Release/comboom.sucht Live Wallpaper.app"
```

## CMake Configuration Options

| Option | Example | Purpose |
|--------|---------|---------|
| `-DCMAKE_BUILD_TYPE` | `Release` \| `Debug` | Optimization level |
| `-DGLFW_USE_X11` | `ON` | Force X11 (Linux) |
| `-DGLFW_USE_WAYLAND` | `ON` | Force Wayland (Linux) |
| `-DCMAKE_C_COMPILER` | `gcc-14` | C compiler |
| `-DCMAKE_CXX_COMPILER` | `g++-14` | C++ compiler |
| `-DCMAKE_C_FLAGS` | `-m32` | Additional C flags (32-bit) |
| `-DCMAKE_CXX_FLAGS` | `-m32` | Additional C++ flags (32-bit) |
| `-DCMAKE_SYSTEM_NAME` | `Linux` | Cross-compile target OS |
| `-DCMAKE_SYSTEM_PROCESSOR` | `aarch64` | Target architecture |

## Troubleshooting

### CMake not found
```bash
# Linux
sudo apt install cmake

# macOS
brew install cmake

# Windows
choco install cmake
```

### Clang / Clang++ not found
```bash
# Verify version
clang --version  # Should be 17+
clang++ --version

# Linux - Update to Clang 18
sudo apt install clang-18 clang++-18
sudo update-alternatives --install /usr/bin/clang clang /usr/bin/clang-18 100
sudo update-alternatives --install /usr/bin/clang++ clang++ /usr/bin/clang++-18 100

# macOS
brew install llvm

# Windows
choco install llvm
```

### Vulkan SDK not found
```bash
# Linux
sudo apt install vulkan-tools libvulkan-dev

# Verify
vulkaninfo

# macOS
brew install vulkan-sdk

# Windows
# Download from https://www.lunarg.com/vulkan-sdk/
# Or: choco install vulkan-sdk
```

### GLFW not found
```bash
# Linux
sudo apt install libglfw3-dev
pkg-config --modversion glfw3

# macOS
brew install glfw

# Verify
pkg-config --modversion glfw3
```

### Shader compilation fails (glslc not found)
```bash
# Linux
sudo apt install glslang-tools
which glslc

# macOS
brew install glslang
which glslc

# Add to PATH if needed
export PATH="$PATH:/usr/local/vulkan/bin"
```

### Cross-compilation target not found
```bash
# For ARM64 on Linux
sudo apt install gcc-aarch64-linux-gnu g++-aarch64-linux-gnu

# Verify
aarch64-linux-gnu-gcc --version
```

### Ninja build system not found
```bash
# Linux
sudo apt install ninja-build

# macOS
brew install ninja

# Windows
choco install ninja
```

## Development Setup

### Generate compile_commands.json for IDE
```bash
cd Linux
cmake -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
# Copy to project root for LSP integration
cp build/compile_commands.json ..
```

### Code Formatting
```bash
# Using clang-format
find Linux/src Windows/src -type f \( -name "*.hpp" -o -name "*.cpp" \) | \
    xargs clang-format -i --style=google
```

### Install clang-format
```bash
# Linux
sudo apt install clang-format

# macOS
brew install clang-format

# Windows
choco install llvm
```

## Next Steps

1. **Implement C++ wrapper** (`Linux/src/wallpaper_engine.hpp/.cpp`) - RAII wrapper around C-core
2. **Implement window manager** (`Linux/src/window_manager.hpp/.cpp`) - GLFW abstraction
3. **Implement Vulkan renderer** (`Linux/src/vulkan_renderer.hpp/.cpp`) - GPU rendering pipeline
4. **Port shaders** (`Linux/src/shaders/particle.vert/frag.glsl`) - From Metal to GLSL
5. **System tray integration** - AppIndicator3 (Linux), Windows Shell API (Windows)
6. **Desktop integration** - X11/_NET_WM_, Wayland layer-shell

See [Implementation Plan](/.claude/plans/dapper-humming-sun.md) for detailed technical roadmap.

---

## Release Builds

Automated release builds for all platforms via GitHub Actions:

```bash
# Create a release tag
git tag -a v1.0.6 -m "Release version 1.0.6"
git push origin v1.0.6

# Go to GitHub Releases → Create Release from tag
# Workflow automatically builds all platforms + architectures
```

See [RELEASE.md](RELEASE.md) for detailed release workflow.

---

## Performance & Optimization

### Build Optimization
- Default: `-O3` (maximum optimization)
- To enable Link-Time Optimization (slower build, faster binary):
  ```bash
  -DCMAKE_CXX_FLAGS="-flto"
  ```

### Runtime Performance
- Particle count: 100-1000 (configurable)
- Target: 60 FPS with 500 particles
- GPU: Vulkan multi-buffering with VMA

### Compiler choice
- **Clang 18**: Default, best C++23 support + faster compilation
- **GCC 13+**: Alternative for better diagnostics
- **Apple Clang**: macOS native (via Xcode)

---

**Last Updated**: 2026-04-06
**Primary Compiler**: Clang 18 (C++23)
**Build System**: CMake 3.20+
**Architectures**: x86-64, ARM64, x86 (32-bit)
