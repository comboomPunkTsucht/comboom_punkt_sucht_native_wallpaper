#pragma once

#include "raylib.h"
#include <string>

namespace AssetsLoader {
    // Asset loading from disk
    // Assets should be in "assets/" subdirectory relative to executable

    // Font loading - returns Font with fallback if not found
    Font load_font(const std::string& font_path, int font_size);

    // Image loading - returns empty Image with fallback if not found
    Image load_image(const std::string& image_path);

    // Get default asset paths (relative to executable's directory)
    std::string get_assets_dir();

    // Check if assets directory exists
    bool assets_exist();
}
