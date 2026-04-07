#pragma once

#include <cstring>
#include <memory>
#include "../../../core/cbps_wallpaper_engine_core.h"

class WallpaperApp {
public:
    CBPSWallpaperEngine* engine = nullptr;
    std::string h1 = "comboom.sucht";
    std::string h2 = "Live Wallpaper";

    WallpaperApp();
    ~WallpaperApp();

    void init(unsigned int width, unsigned int height, unsigned int particle_count);
    void update(float delta_time, int mouse_x, int mouse_y, unsigned int screen_width, unsigned int screen_height);
    void set_h1(const std::string& text);
    void set_h2(const std::string& text);
    void cleanup();
};
