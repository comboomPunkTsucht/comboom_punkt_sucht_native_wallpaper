# Just Commands - Quick Reference

Complete guide to building comboom. sucht using Justfile.

## Quick Commands

```bash
just              # Show help menu
just info         # Show project information
```

## Building C-Core Library

```bash
# Individual platform libraries
just wasm         # Build WebAssembly (.wasm)
just linux-static # Build Linux static library (.a)
just win-static   # Build Windows static library (.a)
just mac-static   # Build macOS static library (.a)

# All libraries at once
just all
```

## Building Native Apps

### Linux (Clang 18 + Vulkan)

```bash
just linux-vulkan       # x86_64 (default, X11/Wayland)
just linux-vulkan-arm   # ARM64 cross-compile
just linux-all          # All Linux architectures
```

Test locally:
```bash
just run-linux    # Run Linux app with test parameters
```

### Windows (Clang/LLVM + Vulkan)

```bash
just windows-vulkan       # x86_64 (default)
just windows-vulkan-arm   # ARM64 cross-compile
just windows-all          # All Windows architectures
```

Test locally (Windows):
```bash
just run-windows  # Run Windows app with test parameters
```

### macOS (Apple Clang + Metal)

```bash
just macos-metal  # ARM64 (Apple Silicon only)
```

Test locally:
```bash
just run-macos    # Launch macOS app
```

## Convenience Shortcuts

```bash
# Build multiple platforms
just native       # Build all 3 main platforms (x86_64): Linux, Windows, macOS
just all-platforms # Build all architectures (9 total binaries)
just rebuild      # Clean + rebuild everything

# Short aliases (type less!)
just l            # = just linux-vulkan
just w            # = just windows-vulkan
just m            # = just macos-metal
just b            # = just native
just ba           # = just all-platforms
just c            # = just clean
```

## Cleanup

```bash
just clean        # Remove all build artifacts
```

## Project Info

```bash
just info         # Show project details (compiler, versions, etc.)
```

---

## Build Matrix

| Command | Target | Arch | Compiler | Output |
|---------|--------|------|----------|--------|
| `just linux-vulkan` | Linux | x86_64 | Clang 18 | `Linux/build/bin/...` |
| `just linux-vulkan-arm` | Linux | ARM64 | aarch64-linux-gnu-clang | `Linux/build-arm/bin/...` |
| `just windows-vulkan` | Windows | x86_64 | LLVM | `Windows/build/bin/...` |
| `just windows-vulkan-arm` | Windows | ARM64 | LLVM | `Windows/build-arm/bin/...` |
| `just macos-metal` | macOS | ARM64 | Apple Clang | `MacOS/build/Release/...` |

## Common Workflows

### Development Build (Linux)
```bash
just linux-vulkan
just run-linux
```

### Release Build (All Platforms)
```bash
just rebuild
just all-platforms
# Artifacts ready for GitHub Release
```

### Cross-Compile for ARM64
```bash
# On x86_64 Linux, build for ARM64
just linux-vulkan-arm

# On x86_64 Windows, build for ARM64
just windows-vulkan-arm
```

### Quick Rebuild
```bash
just c      # Clean
just b      # Build native apps
```

---

## Environment Variables

To use custom Clang version:
```bash
CC=clang-17 CXX=clang++-17 just linux-vulkan
```

To use GCC (fallback):
```bash
CC=gcc-13 CXX=g++-13 just linux-vulkan
```

---

## Troubleshooting

### Command not found: clang-18
Install Clang 18:
- **Linux**: `sudo apt install clang-18 clang++-18`
- **macOS**: `brew install llvm`
- **Windows**: `choco install llvm`

### Cross-compile tools missing (ARM64)
```bash
# Linux ARM64 cross-compile needs:
sudo apt install gcc-aarch64-linux-gnu binutils-aarch64-linux-gnu
```

### Ninja not found
```bash
sudo apt install ninja-build    # Linux
brew install ninja              # macOS
choco install ninja             # Windows
```

---

## Complete Build Sequence (CI/Release)

```bash
# 1. Clean everything
just c

# 2. Build all libraries
just libs:all

# 3. Build all native apps (5 architectures)
just all-platforms

# Result: 5 artifacts ready for GitHub Release
```

This is equivalent to running GitHub Actions workflow locally!

---

**Last Updated**: 2026-04-06
**Just Version**: 1.48+
**Primary Compiler**: Clang 18 (C++23)
