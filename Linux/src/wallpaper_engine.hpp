#pragma once

#include <vector>
#include <memory>
#include <string>
#include <cstdint>

// Include C core engine
extern "C" {
#include "../core/cbps_wallpaper_engine_core.h"
}

namespace cbps {

/**
 * @class Particle
 * @brief C++ wrapper for a single particle
 */
struct Particle {
    float x, y;                 // Current position
    float translateX, translateY;  // Mouse offset (smooth)
    float dx, dy;              // Drift velocity
    float magnetism;           // Mouse attraction (0.1-4.1)
    int radius;                // Particle radius

    struct Color {
        uint8_t r, g, b, a;
    } color;
};

/**
 * @class WallpaperEngine
 * @brief RAII wrapper around the C-based particle engine
 *
 * Manages the lifecycle of CBPSWallpaperEngine, providing a clean C++ interface
 * with automatic resource cleanup via RAII idiom.
 */
class WallpaperEngine {
public:
    /**
     * @brief Color struct - RGBA color representation
     */
    struct Color {
        uint8_t r, g, b, a;
    };

    /**
     * @brief Initialize the wallpaper engine
     * @param width Screen width in pixels
     * @param height Screen height in pixels
     * @param particle_count Number of particles to create
     * @param h1 Primary text (title)
     * @param h2 Secondary text (subtitle)
     */
    WallpaperEngine(uint32_t width, uint32_t height, uint32_t particle_count,
                    const std::string& h1, const std::string& h2);

    /**
     * @brief Destructor - cleans up C engine resources
     */
    ~WallpaperEngine();

    // Non-copyable, movable
    WallpaperEngine(const WallpaperEngine&) = delete;
    WallpaperEngine& operator=(const WallpaperEngine&) = delete;
    WallpaperEngine(WallpaperEngine&&) noexcept;
    WallpaperEngine& operator=(WallpaperEngine&&) noexcept;

    /**
     * @brief Update engine state for one frame
     * @param delta_time Time elapsed since last frame (seconds)
     * @param h1 Primary text
     * @param h2 Secondary text
     * @param width Current screen width
     * @param height Current screen height
     * @param mouse_x Mouse X position
     * @param mouse_y Mouse Y position
     */
    void update(float delta_time, const std::string& h1, const std::string& h2,
                uint32_t width, uint32_t height, int32_t mouse_x, int32_t mouse_y);

    /**
     * @brief Get current particle data
     * @return Vector of particles for rendering
     */
    std::vector<Particle> getParticles() const;

    /**
     * @brief Set random seed for particle generation
     * @param seed Random seed value
     */
    void setSeed(uint32_t seed);

    /**
     * @brief Check if engine is valid
     * @return true if engine was successfully initialized
     */
    bool isValid() const { return engine_ != nullptr; }

    /**
     * @brief Get background color
     */
    Color getBackgroundColor() const;

    /**
     * @brief Get foreground color
     */
    Color getForegroundColor() const;

private:
    CBPSWallpaperEngine* engine_;
};

} // namespace cbps
