#pragma once

#include <string>
#include <functional>

class SystemTray {
public:
    using TextInputCallback = std::function<void(const std::string& text)>;
    using QuitCallback = std::function<void()>;

    virtual ~SystemTray() = default;

    // Initialize system tray with callbacks
    virtual bool init(const std::string& app_name, const std::string& icon_path) = 0;

    // Set menu item callbacks
    virtual void set_h1_callback(TextInputCallback callback) = 0;
    virtual void set_h2_callback(TextInputCallback callback) = 0;
    virtual void set_quit_callback(QuitCallback callback) = 0;

    // Update text display in menu
    virtual void set_h1_text(const std::string& text) = 0;
    virtual void set_h2_text(const std::string& text) = 0;

    // Main event loop
    virtual void run() = 0;

    // Request quit
    virtual void quit() = 0;

    // Check if should quit
    virtual bool should_quit() const = 0;

    // Static factory
    static std::unique_ptr<SystemTray> create();
};
