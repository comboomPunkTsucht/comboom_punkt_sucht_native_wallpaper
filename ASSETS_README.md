# comboom. sucht Live Wallpaper - Assets

## Installation Quick Start

1. **Extract the archive** (wherever you want)
   ```bash
   tar -xzf comboom-sucht-linux-x64.tar.gz
   # OR on Windows
   unzip comboom-sucht-windows-x64.zip
   ```

2. **Make sure the `assets/` folder is in the same directory** as the executable
   ```
   comboom-sucht-linux-x64          (or .exe on Windows)
   assets/
     ├── Logo.png                   (main application icon)
     ├── fonts/
     │   ├── CaskaydiaCoveNerdFontPropo-Regular.ttf
     │   ├── CaskaydiaCoveNerdFontPropo-Bold.ttf
     │   ├── CaskaydiaCoveNerdFontPropo-Light.ttf
     │   └── CaskaydiaCoveNerdFontPropo-ExtraLight.ttf
     └── pictures/
         ├── bd_logo.png
         ├── cbps_logo.png
         ├── fabelke_logo.png
         ├── knuddelzwerck_logo.png
         └── mahd_logo.png
   ```

3. **Run the wallpaper**
   ```bash
   ./comboom-sucht-linux-x64
   # or on Windows
   comboom-sucht-windows-x64.exe
   ```

## About Assets

The wallpaper includes custom fonts and logo images used for rendering:

- **Logo.png**: Main application icon (used in system tray, window title bar, etc.)
- **Fonts**: CaskaydiaCove Nerd Font Proportional (variants: Regular, Bold, Light, ExtraLight)
- **Pictures**: Brand logos from collaborators (bd, cbps, fabelke, knuddelzwerck, mahd)

## Customization

You can customize the wallpaper by editing the files in the `assets/` folder:

- **Replace Logo.png**: Use any PNG image as the app icon/logo
- **Replace fonts**: Drop your own `.ttf` font files in `assets/fonts/`
- **Replace pictures**: Drop your own `.png` image files in `assets/pictures/`

The app will automatically use the files and gracefully fall back to default fonts if custom ones aren't found.

## Troubleshooting

| Problem | Solution |
|---------|----------|
| "No assets found" warning | Make sure `assets/` folder is in the same directory as the executable |
| Custom fonts not working | Ensure font files are `.ttf` format and readable |
| Missing logos in tray menu | The app works fine without picture assets - they're optional |
| Logo not displayed in tray | Verify Logo.png is readable and in a standard format (24x24 or 32x32 preferred) |
| Wallpaper won't start | Check that the executable has execute permissions (`chmod +x` on Linux) |

## Advanced: Running from Different Directory

If you want to run the wallpaper from a different directory, you can create a symlink to the assets folder:

```bash
# From the installation directory (where comboom-sucht-* is)
/path/to/wallpaper/comboom-sucht-linux-x64
```

Or set the working directory when launching:
```bash
cd /path/to/wallpaper && ./comboom-sucht-linux-x64
```

---

**Need help?** Check the README.md in the main repository or create an issue on GitHub.
