# Release Process & GitHub Actions

## 🚀 Automated Release Builds

The project uses GitHub Actions to automatically build and release native apps for all platforms.

### Workflow: `release-native-apps.yml`

**Triggers on**: GitHub release publication
**Builds**: Linux (Vulkan), Windows (Vulkan), macOS (Metal)
**Outputs**: Platform-specific archives uploaded as release assets

---

## 📋 Creating a Release

### Step 1: Update Version Numbers

Update version in key files:
- `package.json` - npm version
- `MacOS/{project}.xcodeproj/project.pbxproj` - Xcode version
- `core/cbps_wallpaper_engine_core.h` - Core version define (if applicable)

```bash
# Ensure everything is committed
git status
git add -A
git commit -m "chore: bump version to 1.0.6"
```

### Step 2: Create Git Tag

```bash
git tag -a v1.0.6 -m "Release version 1.0.6 - Add feature X, fix bug Y"
git push origin v1.0.6
```

### Step 3: Create GitHub Release

**Option A: Via GitHub Web**
1. Go to [Releases](../../releases)
2. Click "Draft a new release"
3. Select your tag (e.g., `v1.0.6`)
4. Title: `v1.0.6 - Brief Description`
5. Description: Include changelog, features, fixes
6. Click "Publish release" ✨

**Option B: Via GitHub CLI**
```bash
gh release create v1.0.6 \
  --title "v1.0.6 - Feature Complete" \
  --notes "See CHANGELOG.md for details"
```

---

## 🔨 What the Workflow Does

### For Each Platform (Parallel)

```
ubuntu-latest (Linux)
├─ Install GLFW, Vulkan SDK, glslang, AppIndicator3
├─ Build C-core: just libs:linux
├─ Build app: CMake + Vulkan
└─ Create: comboom-sucht-linux-x64.tar.gz

windows-latest (Windows)
├─ Install CMake, Vulkan SDK
├─ Build C-core: just libs:windows
├─ Build app: Visual Studio + Vulkan
└─ Create: comboom-sucht-windows-x64.zip

macos-latest (macOS)
├─ Install CMake, Vulkan SDK, glslang
├─ Build C-core: just libs:macos
├─ Build app: Xcode + Metal
└─ Create: comboom-sucht-macos-arm64.zip
```

### Upload Assets
- All archives automatically uploaded to GitHub Release
- Release notes auto-generated with platform info & download links

---

## 📥 Installation for End Users

### Linux
```bash
# Download comboom-sucht-linux-x64.tar.gz from releases
tar -xzf comboom-sucht-linux-x64.tar.gz
./comboom_punkt_sucht_wallpaper --h1 "My Text" --h2 "Subtitle"
```

**Requirements**:
- X11 or Wayland session
- Vulkan-capable GPU + drivers

### Windows
```cmd
# Download comboom-sucht-windows-x64.zip from releases
# Extract to desired location
comboom_punkt_sucht_wallpaper.exe --h1 "My Text" --h2 "Subtitle"
```

**Requirements**:
- Windows 10 or later
- Vulkan-capable GPU + drivers

### macOS
```bash
# Download comboom-sucht-macos-arm64.zip from releases
unzip comboom-sucht-macos-arm64.zip
open "comboom.sucht Live Wallpaper.app"
```

**Requirements**:
- macOS 12+ (tested on Apple Silicon)
- Metal GPU support

---

## 🔍 Monitoring Builds

### Check Workflow Status
1. Go to [Actions](../../actions)
2. Select "Release Native Apps" workflow
3. View latest run details

### Troubleshooting Build Failures

**Common Issues**:

| Issue | Solution |
|-------|----------|
| GLFW not found (Linux) | `sudo apt-get install libglfw3-dev` |
| Vulkan SDK not in PATH | Add to PATH: `/usr/local/vulkan/bin` |
| Windows compilation fails | Ensure Visual Studio 2019+ installed with C++ workload |
| macOS Metal build fails | Update Xcode: `xcode-select --install` |

**View Logs**:
- Click on failed workflow run
- Click on failed job (e.g., "Build linux")
- Expand log sections to see error details

---

## 🏗️ Build Artifacts & Naming

### Archive Format
```
# Linux
comboom-sucht-linux-x64.tar.gz
├─ comboom_punkt_sucht_wallpaper     (executable)

# Windows
comboom-sucht-windows-x64.zip
├─ comboom_punkt_sucht_wallpaper.exe (executable)

# macOS
comboom-sucht-macos-arm64.zip
└─ comboom.sucht Live Wallpaper.app/ (macOS app bundle)
    ├─ MacOS/comboom.sucht Live Wallpaper (executable)
    ├─ Resources/...
    └─ Info.plist
```

### Version in Archives
All artifacts are named with platform + architecture for clarity:
- `linux-x64` → Linux 64-bit (x86-64)
- `windows-x64` → Windows 64-bit (x86-64)
- `macos-arm64` → macOS Apple Silicon (ARM64)

---

## 📝 Release Checklist

- [ ] Update version numbers in all files
- [ ] Verify C-core library builds correctly
- [ ] Test native app locally on each platform
- [ ] Update CHANGELOG.md or release notes
- [ ] Create git tag with format `v#.#.#`
- [ ] Push tag to origin: `git push origin v#.#.#`
- [ ] Create GitHub Release from tag
- [ ] ✨ Workflow automatically builds & uploads artifacts
- [ ] Verify all platforms present in release
- [ ] Test downloaded binaries work correctly

---

## 🔐 GitHub Actions Secrets

No special secrets required for this workflow. It uses:
- `GITHUB_TOKEN` (automatically provided by GitHub)
- Public artifact uploads to releases

**Note**: If you add private dependencies later, add secrets in:
Settings → Secrets and variables → Actions

---

## 🎯 Continuous Integration

This workflow is **release-specific**. For continuous builds on every push/PR, see:
- [CI/CD Guide](./CI.md) (if created)
- `.github/workflows/` directory for additional workflows

---

## 🤝 Contributing

When submitting PRs:
1. Code is built in PR checks (if enabled)
2. Manual test before creating release
3. Follow semantic versioning: `vMAJOR.MINOR.PATCH`

See [CONTRIBUTING.md](../CONTRIBUTING.md) for detailed guidelines.

---

## 📞 Support

Having issues?
- Check [Troubleshooting](#troubleshooting-build-failures) section
- Open an [issue](../../issues) with workflow logs
- See [BUILD.md](../BUILD.md) for local build help

---

**Last Updated**: 2026-04-06
**Workflow File**: `.github/workflows/release-native-apps.yml`
