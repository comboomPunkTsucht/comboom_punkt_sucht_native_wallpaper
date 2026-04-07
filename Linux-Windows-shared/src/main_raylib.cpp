#include "raylib.h"
#include "wallpaper_app.hpp"
#include "renderer_raylib.hpp"
#include "system_tray.hpp"
#include "assets_loader.hpp"
#include <iostream>
#include <thread>
#include <mutex>

// Global state for the Raylib window thread and system tray thread communication
struct AppState {
    WallpaperApp* app = nullptr;
    RendererRaylib* renderer = nullptr;
    std::string h1_text = "comboom.sucht";
    std::string h2_text = "Live Wallpaper";
    std::mutex state_mutex;
    bool should_quit = false;
};

AppState g_app_state;

void raylib_thread_main() {
    if (!g_app_state.app) return;

    // Initialize Raylib window
    const int screen_width = 1920;
    const int screen_height = 1080;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(screen_width, screen_height, "comboom.sucht Live Wallpaper");
    SetTargetFPS(60);

    // Create renderer
    RendererRaylib renderer(g_app_state.app->engine);
    g_app_state.renderer = &renderer;

    // Load initial engine state
    g_app_state.app->init(screen_width, screen_height, 5000);  // 5000 particles

    // Main render loop
    while (!WindowShouldClose() && !g_app_state.should_quit) {
        // Get frame time for physics update
        float dt = GetFrameTime();

        // Get mouse position
        int mx = GetMouseX();
        int my = GetMouseY();

        // Lock to read current H1/H2 from system tray
        {
            std::lock_guard<std::mutex> lock(g_app_state.state_mutex);
            g_app_state.app->set_h1(g_app_state.h1_text);
            g_app_state.app->set_h2(g_app_state.h2_text);
        }

        // Update physics
        g_app_state.app->update(dt, mx, my, GetScreenWidth(), GetScreenHeight());

        // Render
        BeginDrawing();
        ClearBackground(BLACK);

        // Read H1/H2 again for rendering
        std::string h1, h2;
        {
            std::lock_guard<std::mutex> lock(g_app_state.state_mutex);
            h1 = g_app_state.h1_text;
            h2 = g_app_state.h2_text;
        }

        renderer.render(h1, h2);

        EndDrawing();
    }

    // Cleanup
    g_app_state.renderer = nullptr;
    CloseWindow();
}

int main(int argc, char* argv[]) {
    try {
        // Create application
        g_app_state.app = new WallpaperApp();

        // Start Raylib rendering thread
        std::thread raylib_thread(raylib_thread_main);

        // Give Raylib a moment to initialize
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        // Create and initialize system tray
        auto tray = SystemTray::create();
        if (!tray->init("comboom.sucht Live Wallpaper", "")) {
            std::cerr << "Failed to initialize system tray" << std::endl;
            raylib_thread.join();
            delete g_app_state.app;
            return 1;
        }

        // Set up system tray callbacks
        tray->set_h1_callback([&tray](const std::string& text) {
            std::lock_guard<std::mutex> lock(g_app_state.state_mutex);
            g_app_state.h1_text = text;
            tray->set_h1_text(text);  // Update display
        });

        tray->set_h2_callback([&tray](const std::string& text) {
            std::lock_guard<std::mutex> lock(g_app_state.state_mutex);
            g_app_state.h2_text = text;
            tray->set_h2_text(text);
        });

        tray->set_quit_callback([&]() {
            g_app_state.should_quit = true;
        });

        // Run system tray (blocks until quit)
        tray->run();

        // Signal Raylib thread to quit
        g_app_state.should_quit = true;

        // Wait for Raylib thread to finish
        raylib_thread.join();

        // Cleanup
        if (g_app_state.app) {
            g_app_state.app->cleanup();
            delete g_app_state.app;
            g_app_state.app = nullptr;
        }

        return 0;

    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        if (g_app_state.app) {
            delete g_app_state.app;
        }
        return 1;
    }
}
