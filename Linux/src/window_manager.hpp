#pragma once

#include <memory>
#include <string>
#include <cstdint>
#include <utility>

// Forward declare GLFW types
struct GLFWwindow;

namespace cbps {

/**
 * @class WindowManager
 * @brief Abstracts GLFW window management for cross-platform support
 *
 * Provides window creation, event handling, and input management
 * for both Linux (X11/Wayland) and Windows.
 */
class WindowManager {
public:
    /**
     * @brief Window configuration options
     */
    struct Config {
        uint32_t width = 1920;
        uint32_t height = 1080;
        std::string title = "comboom. sucht";
        bool borderless = true;
        bool always_on_top = false;
        bool transparent = true;
    };

    /**
     * @brief Initialize window manager and create window
     * @param config Window configuration
     */
    explicit WindowManager(const Config& config = {});

    /**
     * @brief Destructor - destroys GLFW window
     */
    ~WindowManager();

    // Non-copyable
    WindowManager(const WindowManager&) = delete;
    WindowManager& operator=(const WindowManager&) = delete;

    // Movable
    WindowManager(WindowManager&&) noexcept;
    WindowManager& operator=(WindowManager&&) noexcept;

    /**
     * @brief Poll events and update window state
     * @return true if window should continue, false if should close
     */
    bool update();

    /**
     * @brief Check if window should close
     */
    bool shouldClose() const;

    /**
     * @brief Get current mouse position
     * @return {x, y} mouse coordinates in window space
     */
    std::pair<int32_t, int32_t> getMouse() const;

    /**
     * @brief Get current window width
     */
    uint32_t getWidth() const { return width_; }

    /**
     * @brief Get current window height
     */
    uint32_t getHeight() const { return height_; }

    /**
     * @brief Get underlying GLFW window handle
     * @return GLFWwindow pointer (for Vulkan surface creation, etc.)
     */
    GLFWwindow* getHandle() const { return window_; }

    /**
     * @brief Wait for events (blocks until event occurs or timeout)
     * @param timeout_ms Timeout in milliseconds (0 = no timeout)
     */
    void waitEvents(int timeout_ms = 0) const;

    /**
     * @brief Check if window is valid
     */
    bool isValid() const { return window_ != nullptr; }

private:
    GLFWwindow* window_;
    uint32_t width_;
    uint32_t height_;
    int32_t mouse_x_;
    int32_t mouse_y_;

    void updateMousePosition();
};

} // namespace cbps
