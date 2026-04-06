#include "window_manager.hpp"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <stdexcept>
#include <iostream>

namespace cbps {

// Global GLFW initialization tracker
static bool g_glfw_initialized = false;

WindowManager::WindowManager(const Config& config)
    : window_(nullptr), width_(config.width), height_(config.height),
      mouse_x_(0), mouse_y_(0)
{
    // Initialize GLFW once
    if (!g_glfw_initialized) {
        if (!glfwInit()) {
            throw std::runtime_error("Failed to initialize GLFW");
        }
        g_glfw_initialized = true;
    }

    // Configure GLFW window hints for Vulkan (no OpenGL)
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);

    if (config.borderless) {
        glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
    }

    if (config.transparent) {
        glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);
    }

    // Create window
    window_ = glfwCreateWindow(static_cast<int>(config.width),
                              static_cast<int>(config.height),
                              config.title.c_str(),
                              nullptr, nullptr);

    if (!window_) {
        throw std::runtime_error("Failed to create GLFW window");
    }

    // Platform-specific setup
#ifdef __linux__
    // X11/Wayland will be handled by GLFW automatically
    // Set window hints for desktop integration
    // TODO: Set _NET_WM_WINDOW_TYPE on X11 if needed
#elif _WIN32
    // Windows-specific setup will be done in separate platform layer
#endif

    // Store user pointer for callbacks
    glfwSetWindowUserPointer(window_, this);

    // Set resize callback
    glfwSetWindowSizeCallback(window_, [](GLFWwindow* win, int w, int h) {
        auto* self = static_cast<WindowManager*>(glfwGetWindowUserPointer(win));
        self->width_ = static_cast<uint32_t>(w);
        self->height_ = static_cast<uint32_t>(h);
    });

    updateMousePosition();
}

WindowManager::~WindowManager()
{
    if (window_) {
        glfwDestroyWindow(window_);
        window_ = nullptr;
    }
}

WindowManager::WindowManager(WindowManager&& other) noexcept
    : window_(other.window_), width_(other.width_), height_(other.height_),
      mouse_x_(other.mouse_x_), mouse_y_(other.mouse_y_)
{
    other.window_ = nullptr;
    if (window_) {
        glfwSetWindowUserPointer(window_, this);
    }
}

WindowManager& WindowManager::operator=(WindowManager&& other) noexcept
{
    if (this != &other) {
        if (window_) {
            glfwDestroyWindow(window_);
        }
        window_ = other.window_;
        width_ = other.width_;
        height_ = other.height_;
        mouse_x_ = other.mouse_x_;
        mouse_y_ = other.mouse_y_;

        other.window_ = nullptr;

        if (window_) {
            glfwSetWindowUserPointer(window_, this);
        }
    }
    return *this;
}

bool WindowManager::update()
{
    if (!window_) {
        return false;
    }

    glfwPollEvents();
    updateMousePosition();

    return !glfwWindowShouldClose(window_);
}

bool WindowManager::shouldClose() const
{
    if (!window_) {
        return true;
    }
    return glfwWindowShouldClose(window_);
}

std::pair<int32_t, int32_t> WindowManager::getMouse() const
{
    return {mouse_x_, mouse_y_};
}

void WindowManager::waitEvents(int timeout_ms) const
{
    if (!window_) {
        return;
    }

    if (timeout_ms > 0) {
        glfwWaitEventsTimeout(timeout_ms / 1000.0);
    } else {
        glfwWaitEvents();
    }
}

void WindowManager::updateMousePosition()
{
    if (!window_) {
        return;
    }

    double x, y;
    glfwGetCursorPos(window_, &x, &y);

    mouse_x_ = static_cast<int32_t>(x);
    mouse_y_ = static_cast<int32_t>(y);
}

} // namespace cbps
