#include "wallpaper_app.hpp"

WallpaperApp::WallpaperApp() = default;

WallpaperApp::~WallpaperApp() {
    cleanup();
}

void WallpaperApp::init(unsigned int width, unsigned int height, unsigned int particle_count) {
    engine = cbps_engine_create(width, height, particle_count, h1.c_str(), h2.c_str());
}

void WallpaperApp::update(float delta_time, int mouse_x, int mouse_y, unsigned int screen_width, unsigned int screen_height) {
    if (!engine) return;

    cbps_engine_update(engine, delta_time, h1.c_str(), h2.c_str(), screen_width, screen_height, mouse_x, mouse_y);
}

void WallpaperApp::set_h1(const std::string& text) {
    h1 = text;
}

void WallpaperApp::set_h2(const std::string& text) {
    h2 = text;
}

void WallpaperApp::cleanup() {
    if (engine) {
        cbps_engine_destroy(engine);
        engine = nullptr;
    }
}
