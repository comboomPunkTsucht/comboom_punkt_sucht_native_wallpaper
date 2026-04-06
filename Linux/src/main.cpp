#include "wallpaper_engine.hpp"
#include "window_manager.hpp"
#include "vulkan_renderer.hpp"
#include <iostream>
#include <chrono>
#include <cstring>

using namespace cbps;

int main(int argc, char* argv[])
{
    std::cout << "═════════════════════════════════════════════" << std::endl;
    std::cout << "  comboom. sucht - Linux Vulkan Wallpaper" << std::endl;
    std::cout << "  v1.0.5" << std::endl;
    std::cout << "═════════════════════════════════════════════" << std::endl;

    try {
        // Parse command line arguments
        std::string h1 = "comboom.";
        std::string h2 = "sucht";

        for (int i = 1; i < argc - 1; ++i) {
            if (std::strcmp(argv[i], "--h1") == 0) {
                h1 = argv[++i];
            } else if (std::strcmp(argv[i], "--h2") == 0) {
                h2 = argv[++i];
            }
        }

        std::cout << "\n✓ Initializing components..." << std::endl;

        // Create window
        WindowManager::Config window_config{
            .width = 1920,
            .height = 1080,
            .title = "comboom. sucht",
            .borderless = true,
            .always_on_top = false,
            .transparent = true
        };

        WindowManager window(window_config);
        std::cout << "✓ Window created (" << window.getWidth() << "x" << window.getHeight() << ")"
                  << std::endl;

        // Create Vulkan renderer
        VulkanRenderer renderer(window.getHandle(), window.getWidth(), window.getHeight());

        // Create wallpaper engine
        WallpaperEngine engine(window.getWidth(), window.getHeight(), 100, h1, h2);
        std::cout << "✓ Wallpaper engine created (100 particles)" << std::endl;

        // Main render loop
        std::cout << "\n🎬 Starting render loop..." << std::endl;
        std::cout << "  Press ESC to quit" << std::endl;
        std::cout << "  ──────────────────────" << std::endl;

        auto last_time = std::chrono::high_resolution_clock::now();
        uint32_t frame_count = 0;
        auto last_fps_time = last_time;

        while (window.update() && !window.shouldClose()) {
            // Calculate delta time
            auto now = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(now - last_time);
            float delta_time = duration.count() / 1e9f;  // Convert to seconds
            last_time = now;

            // Update wallpaper engine
            auto [mouse_x, mouse_y] = window.getMouse();
            engine.update(delta_time, h1, h2, window.getWidth(), window.getHeight(),
                         mouse_x, mouse_y);

            // Get particles and render
            auto particles = engine.getParticles();
            auto bg_color = engine.getBackgroundColor();
            auto fg_color = engine.getForegroundColor();

            uint8_t bg[] = {bg_color.r, bg_color.g, bg_color.b, bg_color.a};
            uint8_t fg[] = {fg_color.r, fg_color.g, fg_color.b, fg_color.a};

            renderer.render(particles, bg, fg);

            frame_count++;

            // FPS counter (print every 2 seconds)
            auto fps_duration = std::chrono::duration_cast<std::chrono::seconds>(now - last_fps_time);
            if (fps_duration.count() >= 2) {
                float fps = frame_count / fps_duration.count();
                std::cout << "  FPS: " << static_cast<int>(fps) << " | Particles: " << particles.size()
                          << " | Size: " << window.getWidth() << "x" << window.getHeight()
                          << std::endl;
                frame_count = 0;
                last_fps_time = now;
            }
        }

        std::cout << "\n✓ Shutting down..." << std::endl;
        renderer.waitIdle();

    } catch (const std::exception& e) {
        std::cerr << "\n✗ Error: " << e.what() << std::endl;
        return 1;
    }

    std::cout << "✓ Goodbye!" << std::endl;
    return 0;
}
