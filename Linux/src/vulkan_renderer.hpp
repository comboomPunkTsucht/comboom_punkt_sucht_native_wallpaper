#pragma once

#include <vulkan/vulkan.hpp>
#include <vector>
#include <memory>
#include <cstdint>

// Forward declarations
struct GLFWwindow;
struct VmaAllocator;
struct VmaAllocation;

namespace cbps {

struct Particle;

/**
 * @class VulkanRenderer
 * @brief Manages Vulkan rendering pipeline for particle visualization
 *
 * Handles instance creation, device selection, swapchain management,
 * render pipeline setup, and per-frame rendering.
 */
class VulkanRenderer {
public:
    /**
     * @brief Initialize Vulkan renderer
     * @param window GLFW window handle (for surface creation)
     * @param width Initial window width
     * @param height Initial window height
     */
    VulkanRenderer(GLFWwindow* window, uint32_t width, uint32_t height);

    /**
     * @brief Destructor - cleanly shutdown Vulkan resources
     */
    ~VulkanRenderer();

    // Non-copyable
    VulkanRenderer(const VulkanRenderer&) = delete;
    VulkanRenderer& operator=(const VulkanRenderer&) = delete;

    /**
     * @brief Render a frame with the given particles
     * @param particles Vector of particles to render
     * @param bg_color Background color {r, g, b, a}
     * @param fg_color Foreground color {r, g, b, a}
     * @return true if render succeeded, false if needs format reset
     */
    bool render(const std::vector<Particle>& particles,
               const uint8_t bg_color[4],
               const uint8_t fg_color[4]);

    /**
     * @brief Handle window resize
     * @param new_width New window width
     * @param new_height New window height
     */
    void onResize(uint32_t new_width, uint32_t new_height);

    /**
     * @brief Check if renderer is initialized and valid
     */
    bool isValid() const { return device_; }

    /**
     * @brief Wait for GPU to finish all work
     */
    void waitIdle() const;

private:
    // Vulkan objects
    vk::Instance instance_;
    vk::PhysicalDevice physical_device_;
    vk::Device device_;
    vk::Queue graphics_queue_;
    uint32_t graphics_queue_family_;

    // Surface & Swapchain
    vk::SurfaceKHR surface_;
    vk::SwapchainKHR swapchain_;
    std::vector<vk::Image> swapchain_images_;
    std::vector<vk::ImageView> swapchain_image_views_;
    vk::Format swapchain_format_;
    vk::Extent2D swapchain_extent_;

    // Command buffers
    vk::CommandPool command_pool_;
    std::vector<vk::CommandBuffer> command_buffers_;

    // Render pass & Pipeline
    vk::RenderPass render_pass_;
    vk::PipelineLayout pipeline_layout_;
    vk::Pipeline graphics_pipeline_;
    std::vector<vk::Framebuffer> framebuffers_;

    // Synchronization
    std::vector<vk::Semaphore> image_available_semaphores_;
    std::vector<vk::Semaphore> render_finished_semaphores_;
    std::vector<vk::Fence> in_flight_fences_;
    uint32_t current_frame_;

    // GPU Memory
    VmaAllocator allocator_;
    vk::Buffer particle_buffer_;
    VmaAllocation particle_allocation_;

    // Descriptor sets for shaders
    vk::DescriptorSetLayout descriptor_set_layout_;
    vk::DescriptorPool descriptor_pool_;
    vk::DescriptorSet descriptor_set_;

    // Window dimensions (for device-local extent)
    uint32_t surface_width_;
    uint32_t surface_height_;

    // Helper methods
    void createInstance();
    void selectPhysicalDevice();
    void createLogicalDevice();
    void createMemoryAllocator();
    void createSurface(GLFWwindow* window);
    void createSwapchain();
    void createImageViews();
    void createRenderPass();
    void createDescriptorSetLayout();
    void createGraphicsPipeline();
    void createFramebuffers();
    void createCommandPool();
    void createCommandBuffers();
    void createSynchronizationObjects();
    void createParticleBuffer();

    void recreateSwapchain();
    void cleanupSwapchain();

    vk::ShaderModule loadShaderModule(const std::vector<uint32_t>& code);
};

} // namespace cbps
