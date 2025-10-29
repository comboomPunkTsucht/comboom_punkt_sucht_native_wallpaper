# comboom_punkt_sucht_native_wallpaper

A basic C++23 Qt6 application with CMake and tsoding/flag.h for command-line argument parsing.

## Requirements

- C++23 compatible compiler (GCC 13+, Clang 16+, or MSVC 2022+)
- CMake 3.16 or higher
- Qt6 (Core and Widgets modules)

## Building

### Ubuntu/Debian

Install dependencies:

```bash
sudo apt-get update
sudo apt-get install build-essential
```

Build the project:

```bash
make all
```

### Running

After building, run the application:

```bash
./build/cbps-lwp
```

### Command-line Options

The application supports the following command-line flags (powered by tsoding/flag.h):

- `-V`/`--version`: Show version information
- `-h`/`--help`: Show Help Information
- `--title <string>`: Set Main title (default: "Comboom Punkt Sucht Native Wallpaper")
- `--sub_title <string>`: Set the Sub title
- `--width <int>`: Set window width (default: 1280)
- `--height <int>`: Set window height (default: 720)

Example:

```bash
./build/cbps-lwp --title "My Custom Title" --width 1024 --height 768
```

## Features

- C++23 standard compliance
- Qt6 Widgets for modern GUI
- CMake build system
- Command-line argument parsing with tsoding/flag.h
- Cross-platform support

## License

See LICENSE file for details.
