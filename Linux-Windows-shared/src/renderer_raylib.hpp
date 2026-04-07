#pragma once

#include "raylib.h"
#include "../../../core/cbps_wallpaper_engine_core.h"
#include <cmath>
#include <string>

class RendererRaylib {
private:
    CBPSWallpaperEngine* engine = nullptr;
    Font font_regular = {};
    Font font_bold = {};
    Image logo_image = {};
    Texture2D logo_texture = {};

    // Helper to draw anti-aliased circles for particles
    void draw_particle(const CBPSParticle& particle, float screen_width, float screen_height);

public:
    RendererRaylib(CBPSWallpaperEngine* engine_ptr);
    ~RendererRaylib();

    void render(const std::string& h1_text = "", const std::string& h2_text = "");
    void load_fonts();
    void load_assets();
    void cleanup();
};
