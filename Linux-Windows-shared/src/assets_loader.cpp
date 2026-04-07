#include "assets_loader.hpp"
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

namespace AssetsLoader {

std::string get_assets_dir() {
    // Assets are in "assets/" subdirectory next to the executable
    return "assets";
}

bool assets_exist() {
    return fs::exists(get_assets_dir()) && fs::is_directory(get_assets_dir());
}

Font load_font(const std::string& font_path, int font_size) {
    // Try to load font from file
    if (fs::exists(font_path)) {
        try {
            Font font = LoadFont(font_path.c_str());
            if (font.glyphCount > 0) {
                return font;  // Successfully loaded
            }
        } catch (...) {
            // Fall through to default
        }
    }

    // Fallback: Return default font
    std::cerr << "Warning: Font not found at '" << font_path << "', using default font" << std::endl;
    return GetFontDefault();
}

Image load_image(const std::string& image_path) {
    // Try to load image from file
    if (fs::exists(image_path)) {
        try {
            Image image = LoadImage(image_path.c_str());
            if (image.data != nullptr) {
                return image;  // Successfully loaded
            }
        } catch (...) {
            // Fall through to default
        }
    }

    // Fallback: Return empty image (Raylib will handle gracefully)
    std::cerr << "Warning: Image not found at '" << image_path << "'" << std::endl;
    return { 0 };
}

}  // namespace AssetsLoader
