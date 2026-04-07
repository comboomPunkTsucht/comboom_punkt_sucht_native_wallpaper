#pragma once

#include "raylib.h"
#include <span>
#include <memory>

namespace AssetsLoader {
    // Asset loaders - will load from embedded assets using C++23 #embed
    Font load_font_from_memory(const std::span<const unsigned char>& font_data, int font_size);
    Image load_image_from_memory(const std::span<const unsigned char>& image_data);

    // Logo
    std::span<const unsigned char> get_logo_data();

    // Fonts (Caskaydida Cove Nerd Font variants)
    std::span<const unsigned char> get_font_regular_data();
    std::span<const unsigned char> get_font_bold_data();
    std::span<const unsigned char> get_font_light_data();
    std::span<const unsigned char> get_font_extra_light_data();

    // Picture Logos
    std::span<const unsigned char> get_bd_logo_data();
    std::span<const unsigned char> get_cbps_logo_data();
    std::span<const unsigned char> get_fabelke_logo_data();
    std::span<const unsigned char> get_knuddelzwerck_logo_data();
    std::span<const unsigned char> get_mahd_logo_data();
}
