#include "wallpaper_engine.hpp"
#include "cbps_wallpaper_engine_core.h"

namespace cbps {

WallpaperEngine::WallpaperEngine(uint32_t width, uint32_t height, uint32_t particle_count,
                                 const std::string& h1, const std::string& h2)
    : engine_(nullptr)
{
    // Create C engine
    engine_ = cbps_engine_create(width, height, particle_count,
                                 const_cast<char*>(h1.c_str()),
                                 const_cast<char*>(h2.c_str()));

    if (!engine_) {
        throw std::runtime_error("Failed to create wallpaper engine");
    }
}

WallpaperEngine::~WallpaperEngine()
{
    if (engine_) {
        cbps_engine_destroy(engine_);
        engine_ = nullptr;
    }
}

WallpaperEngine::WallpaperEngine(WallpaperEngine&& other) noexcept
    : engine_(other.engine_)
{
    other.engine_ = nullptr;
}

WallpaperEngine& WallpaperEngine::operator=(WallpaperEngine&& other) noexcept
{
    if (this != &other) {
        if (engine_) {
            cbps_engine_destroy(engine_);
        }
        engine_ = other.engine_;
        other.engine_ = nullptr;
    }
    return *this;
}

void WallpaperEngine::update(float delta_time, const std::string& h1, const std::string& h2,
                            uint32_t width, uint32_t height, int32_t mouse_x, int32_t mouse_y)
{
    if (!engine_) {
        return;
    }

    // Call C engine update
    cbps_engine_update(engine_, delta_time,
                      const_cast<char*>(h1.c_str()),
                      const_cast<char*>(h2.c_str()),
                      width, height, mouse_x, mouse_y);
}

std::vector<Particle> WallpaperEngine::getParticles() const
{
    if (!engine_) {
        return {};
    }

    unsigned int count = 0;
    CBPSParticle* c_particles = cbps_engine_get_particles(engine_, &count);

    if (!c_particles || count == 0) {
        return {};
    }

    std::vector<Particle> particles;
    particles.reserve(count);

    for (unsigned int i = 0; i < count; ++i) {
        const CBPSParticle& src = c_particles[i];
        Particle dst{
            .x = src.x,
            .y = src.y,
            .translateX = src.translateX,
            .translateY = src.translateY,
            .dx = src.dx,
            .dy = src.dy,
            .magnetism = src.magnetism,
            .radius = src.radius,
            .color = {
                .r = src.color.r,
                .g = src.color.g,
                .b = src.color.b,
                .a = src.color.a
            }
        };
        particles.push_back(dst);
    }

    return particles;
}

void WallpaperEngine::setSeed(uint32_t seed)
{
    if (!engine_) {
        return;
    }

    cbps_engine_set_seed(seed);
}

WallpaperEngine::Color WallpaperEngine::getBackgroundColor() const
{
    if (!engine_) {
        return {0, 0, 0, 255};
    }

    const CBPSColor& cc = engine_->background_color;
    return Color{.r = cc.r, .g = cc.g, .b = cc.b, .a = cc.a};
}

WallpaperEngine::Color WallpaperEngine::getForegroundColor() const
{
    if (!engine_) {
        return {255, 255, 255, 255};
    }

    const CBPSColor& cc = engine_->foreground_color;
    return Color{.r = cc.r, .g = cc.g, .b = cc.b, .a = cc.a};
}

} // namespace cbps
