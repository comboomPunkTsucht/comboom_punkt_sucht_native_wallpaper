#include "assets_loader.hpp"
#include <cstring>
#include <array>

// C++23 #embed directives for embedded assets
// Only use on platforms that fully support C++23 #embed
// Windows MinGW-w64 doesn't fully support #embed yet, so we use conditional compilation

#ifdef _WIN32
// Windows: Use empty stub arrays (MinGW-w64 GCC doesn't support #embed in -std=c++23 mode properly)
// Assets are optional for the wallpaper engine - fallback to default rendering
constexpr std::array<unsigned char, 0> logo_data_array = {};
constexpr std::array<unsigned char, 0> font_regular_data_array = {};
constexpr std::array<unsigned char, 0> font_bold_data_array = {};
constexpr std::array<unsigned char, 0> font_light_data_array = {};
constexpr std::array<unsigned char, 0> font_extra_light_data_array = {};
constexpr std::array<unsigned char, 0> bd_logo_data_array = {};
constexpr std::array<unsigned char, 0> cbps_logo_data_array = {};
constexpr std::array<unsigned char, 0> fabelke_logo_data_array = {};
constexpr std::array<unsigned char, 0> knuddelzwerck_logo_data_array = {};
constexpr std::array<unsigned char, 0> mahd_logo_data_array = {};

#else  // Linux and other POSIX systems with full C++23 support

// Logo image (PNG)
constexpr auto logo_data_array = std::to_array<unsigned char>({
    #embed "../../WASM/static/Logo.png"
});

// Font files (TTF) - Caskaydida Cove Nerd Font variants
constexpr auto font_regular_data_array = std::to_array<unsigned char>({
    #embed "../../WASM/static/fonts/CaskaydiaCoveNerdFontPropo-Regular.ttf"
});

constexpr auto font_bold_data_array = std::to_array<unsigned char>({
    #embed "../../WASM/static/fonts/CaskaydiaCoveNerdFontPropo-Bold.ttf"
});

constexpr auto font_light_data_array = std::to_array<unsigned char>({
    #embed "../../WASM/static/fonts/CaskaydiaCoveNerdFontPropo-Light.ttf"
});

constexpr auto font_extra_light_data_array = std::to_array<unsigned char>({
    #embed "../../WASM/static/fonts/CaskaydiaCoveNerdFontPropo-ExtraLight.ttf"
});

// Picture logos
constexpr auto bd_logo_data_array = std::to_array<unsigned char>({
    #embed "../../WASM/static/pictures/bd_logo.png"
});

constexpr auto cbps_logo_data_array = std::to_array<unsigned char>({
    #embed "../../WASM/static/pictures/cbps_logo.png"
});

constexpr auto fabelke_logo_data_array = std::to_array<unsigned char>({
    #embed "../../WASM/static/pictures/fabelke_logo.png"
});

constexpr auto knuddelzwerck_logo_data_array = std::to_array<unsigned char>({
    #embed "../../WASM/static/pictures/knuddelzwerck_logo.png"
});

constexpr auto mahd_logo_data_array = std::to_array<unsigned char>({
    #embed "../../WASM/static/pictures/mahd_logo.png"
});

#endif  // _WIN32

namespace AssetsLoader {

Font load_font_from_memory(const std::span<const unsigned char>& font_data, int font_size) {
    // Raylib's LoadFontFromMemory takes format and data
    // Format for TTF is "ttf"
    // Note: Raylib will copy the data internally
    return LoadFontFromMemory(".ttf", (const unsigned char*)font_data.data(), (int)font_data.size(), font_size, nullptr, 0);
}

Image load_image_from_memory(const std::span<const unsigned char>& image_data) {
    // Raylib's LoadImageFromMemory takes format and data
    // Format for PNG is "png"
    return LoadImageFromMemory(".png", (const unsigned char*)image_data.data(), (int)image_data.size());
}

// Logo
std::span<const unsigned char> get_logo_data() {
    return std::span<const unsigned char>(logo_data_array);
}

// Fonts
std::span<const unsigned char> get_font_regular_data() {
    return std::span<const unsigned char>(font_regular_data_array);
}

std::span<const unsigned char> get_font_bold_data() {
    return std::span<const unsigned char>(font_bold_data_array);
}

std::span<const unsigned char> get_font_light_data() {
    return std::span<const unsigned char>(font_light_data_array);
}

std::span<const unsigned char> get_font_extra_light_data() {
    return std::span<const unsigned char>(font_extra_light_data_array);
}

// Picture logos
std::span<const unsigned char> get_bd_logo_data() {
    return std::span<const unsigned char>(bd_logo_data_array);
}

std::span<const unsigned char> get_cbps_logo_data() {
    return std::span<const unsigned char>(cbps_logo_data_array);
}

std::span<const unsigned char> get_fabelke_logo_data() {
    return std::span<const unsigned char>(fabelke_logo_data_array);
}

std::span<const unsigned char> get_knuddelzwerck_logo_data() {
    return std::span<const unsigned char>(knuddelzwerck_logo_data_array);
}

std::span<const unsigned char> get_mahd_logo_data() {
    return std::span<const unsigned char>(mahd_logo_data_array);
}

}
