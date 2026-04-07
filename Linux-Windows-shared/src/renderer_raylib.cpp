#include "renderer_raylib.hpp"
#include "assets_loader.hpp"

RendererRaylib::RendererRaylib(CBPSWallpaperEngine* engine_ptr) : engine(engine_ptr) {
    load_assets();
    load_fonts();
}

RendererRaylib::~RendererRaylib() {
    cleanup();
}

void RendererRaylib::load_assets() {
    // Assets will be loaded from embedded data by assets_loader
    // For now, we'll load them when rendering needs them
}

void RendererRaylib::load_fonts() {
    // Fonts will be loaded from embedded TTF data
    // This will be called by main.cpp after font loading is set up
}

void RendererRaylib::draw_particle(const CBPSParticle& particle, float screen_width, float screen_height) {
    // Calculate real position (base + mouse offset)
    float real_x = particle.x + particle.translateX;
    float real_y = particle.y + particle.translateY;

    // Normalize to screen coordinates
    float norm_x = (real_x / screen_width) * GetScreenWidth();
    float norm_y = (real_y / screen_height) * GetScreenHeight();

    // Draw smooth circular particle with anti-aliasing via DrawCircle
    // Radius multiplied by 2 because pointSize in Metal is diameter
    float radius = particle.radius * 2.0f;

    // Convert color from CBPSColor to Raylib Color
    Color c = {particle.color.r, particle.color.g, particle.color.b, particle.color.a};

    // Draw the particle as a circle
    DrawCircle((int)norm_x, (int)norm_y, radius, c);
}

void RendererRaylib::render(const std::string& h1_text, const std::string& h2_text) {
    if (!engine) return;

    // Get screen dimensions
    float screen_width = GetScreenWidth();
    float screen_height = GetScreenHeight();

    // Fill background with engine's background color
    CBPSColor bg = engine->background_color;
    Color background = {bg.r, bg.g, bg.b, bg.a};
    DrawRectangle(0, 0, (int)screen_width, (int)screen_height, background);

    // Get particles from engine
    unsigned int particle_count = 0;
    const CBPSParticle* particles = cbps_engine_get_particles(engine, &particle_count);

    // Draw all particles
    if (particles) {
        for (unsigned int i = 0; i < particle_count; ++i) {
            draw_particle(particles[i], engine->screen_width, engine->screen_height);
        }
    }

    // Render text overlays if provided
    if (!h1_text.empty() || !h2_text.empty()) {
        CBPSColor fg = engine->foreground_color;
        Color foreground = {fg.r, fg.g, fg.b, fg.a};

        // H1 text (title)
        if (!h1_text.empty()) {
            int font_size = static_cast<int>((screen_width / 1920.0f) * 60.0f);  // Scale with screen size
            DrawText(h1_text.c_str(), 50, 50, font_size, foreground);
        }

        // H2 text (subtitle)
        if (!h2_text.empty()) {
            int font_size = static_cast<int>((screen_width / 1920.0f) * 40.0f);
            DrawText(h2_text.c_str(), 50, 120, font_size, foreground);
        }
    }
}

void RendererRaylib::cleanup() {
    if (logo_texture.id > 0) {
        UnloadTexture(logo_texture);
    }
    if (logo_image.data) {
        UnloadImage(logo_image);
    }
    if (font_regular.baseSize > 0) {
        UnloadFont(font_regular);
    }
    if (font_bold.baseSize > 0) {
        UnloadFont(font_bold);
    }
}
