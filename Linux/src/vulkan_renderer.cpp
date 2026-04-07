#include "vulkan_renderer.hpp"
#include "wallpaper_engine.hpp"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define VMA_IMPLEMENTATION
#define VMA_STATIC_VULKAN_FUNCTIONS 0
#include <vk_mem_alloc.h>
#include <VkBootstrap.h>
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <array>

namespace cbps {

// Load shader binary from SPIR-V file
static std::vector<uint32_t> loadShaderBinary(const std::string& path)
{
    std::ifstream file(path, std::ios::ate | std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open shader file: " + path);
    }

    size_t file_size = (size_t)file.tellg();
    std::vector<uint32_t> buffer(file_size / sizeof(uint32_t));

    file.seekg(0);
    file.read((char*)buffer.data(), file_size);
    file.close();

    return buffer;
}

VulkanRenderer::VulkanRenderer(GLFWwindow* window, uint32_t width, uint32_t height)
    : physical_device_(nullptr), graphics_queue_family_(0),
      current_frame_(0), allocator_(nullptr), surface_width_(width),
      surface_height_(height)
{
    try {
        createInstance();
        createSurface(window);
        selectPhysicalDevice();
        createLogicalDevice();
        createMemoryAllocator();
        createSwapchain();
        createImageViews();
        createRenderPass();
        createDescriptorSetLayout();
        createGraphicsPipeline();
        createFramebuffers();
        createCommandPool();
        createCommandBuffers();
        createSynchronizationObjects();
        createParticleBuffer();

        std::cout << "✓ Vulkan renderer initialized" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "✗ Vulkan initialization failed: " << e.what() << std::endl;
        throw;
    }
}

VulkanRenderer::~VulkanRenderer()
{
    if (!device_) return;

    try {
        device_.waitIdle();

        // Cleanup resources in reverse order
        if (particle_buffer_) {
            vmaDestroyBuffer(allocator_, particle_buffer_, particle_allocation_);
        }

        for (auto& fence : in_flight_fences_) {
            device_.destroyFence(fence);
        }
        for (auto& sem : image_available_semaphores_) {
            device_.destroySemaphore(sem);
        }
        for (auto& sem : render_finished_semaphores_) {
            device_.destroySemaphore(sem);
        }

        device_.destroyCommandPool(command_pool_);

        for (auto& fb : framebuffers_) {
            device_.destroyFramebuffer(fb);
        }

        device_.destroyPipeline(graphics_pipeline_);
        device_.destroyPipelineLayout(pipeline_layout_);
        device_.destroyRenderPass(render_pass_);
        device_.destroyDescriptorPool(descriptor_pool_);
        device_.destroyDescriptorSetLayout(descriptor_set_layout_);

        cleanupSwapchain();

        if (allocator_) {
            vmaDestroyAllocator(allocator_);
        }

        instance_.destroySurfaceKHR(surface_);
        device_.destroy();
        instance_.destroy();
    } catch (const std::exception& e) {
        std::cerr << "✗ Error during Vulkan cleanup: " << e.what() << std::endl;
    }
}

bool VulkanRenderer::render(const std::vector<Particle>& particles,
                           const uint8_t bg_color[4],
                           const uint8_t fg_color[4])
{
    if (!device_ || particles.empty()) {
        return false;
    }

    // Wait for frame to be finished
    auto fence_result = device_.waitForFences({in_flight_fences_[current_frame_]}, true, UINT64_MAX);
    device_.resetFences({in_flight_fences_[current_frame_]});

    // Acquire next image
    uint32_t image_index;
    auto acquire_result = device_.acquireNextImageKHR(
        swapchain_, UINT64_MAX, image_available_semaphores_[current_frame_], {});

    if (acquire_result.result == vk::Result::eErrorOutOfDateKHR) {
        recreateSwapchain();
        return false;
    } else if (acquire_result.result != vk::Result::eSuccess &&
               acquire_result.result != vk::Result::eSuboptimalKHR) {
        throw std::runtime_error("Failed to acquire swapchain image");
    }

    image_index = acquire_result.value;

    // Update particle buffer (map, copy, unmap)
    void* data = nullptr;
    vmaMapMemory(allocator_, particle_allocation_, &data);
    memcpy(data, particles.data(), particles.size() * sizeof(Particle));
    vmaUnmapMemory(allocator_, particle_allocation_);

    // Record command buffer
    vk::CommandBufferBeginInfo begin_info(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
    command_buffers_[current_frame_].begin(begin_info);

    // Clear color
    vk::ClearColorValue color_value;
    color_value.float32[0] = bg_color[0] / 255.0f;
    color_value.float32[1] = bg_color[1] / 255.0f;
    color_value.float32[2] = bg_color[2] / 255.0f;
    color_value.float32[3] = bg_color[3] / 255.0f;
    vk::ClearValue clear_color(color_value);

    vk::RenderPassBeginInfo render_pass_info(
        render_pass_, framebuffers_[image_index],
        vk::Rect2D(vk::Offset2D(0, 0), swapchain_extent_),
        1, &clear_color);

    command_buffers_[current_frame_].beginRenderPass(render_pass_info, vk::SubpassContents::eInline);
    command_buffers_[current_frame_].bindPipeline(vk::PipelineBindPoint::eGraphics, graphics_pipeline_);

    // Draw particles as points
    command_buffers_[current_frame_].draw(static_cast<uint32_t>(particles.size()), 1, 0, 0);

    command_buffers_[current_frame_].endRenderPass();
    command_buffers_[current_frame_].end();

    // Submit
    vk::PipelineStageFlags wait_stages = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    vk::SubmitInfo submit_info(
        1, &image_available_semaphores_[current_frame_],
        &wait_stages,
        1, &command_buffers_[current_frame_],
        1, &render_finished_semaphores_[current_frame_]);

    graphics_queue_.submit({submit_info}, in_flight_fences_[current_frame_]);

    // Present
    vk::PresentInfoKHR present_info(
        1, &render_finished_semaphores_[current_frame_],
        1, &swapchain_,
        &image_index);

    auto present_result = graphics_queue_.presentKHR(present_info);

    if (present_result == vk::Result::eErrorOutOfDateKHR ||
        present_result == vk::Result::eSuboptimalKHR) {
        recreateSwapchain();
    } else if (present_result != vk::Result::eSuccess) {
        throw std::runtime_error("Failed to present swapchain image");
    }

    current_frame_ = (current_frame_ + 1) % in_flight_fences_.size();
    return true;
}

void VulkanRenderer::onResize(uint32_t new_width, uint32_t new_height)
{
    surface_width_ = new_width;
    surface_height_ = new_height;
    recreateSwapchain();
}

void VulkanRenderer::waitIdle() const
{
    if (device_) {
        device_.waitIdle();
    }
}

// ============= Helper Methods =============

void VulkanRenderer::createInstance()
{
    vk::ApplicationInfo app_info("comboom. sucht Wallpaper",
                                 VK_MAKE_VERSION(1, 0, 5),
                                 "cbps-vulkan",
                                 VK_MAKE_VERSION(1, 0, 0),
                                 VK_API_VERSION_1_2);

    // Get required extensions from GLFW
    uint32_t glfw_extension_count = 0;
    const char** glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);

    std::vector<const char*> extensions(glfw_extensions, glfw_extensions + glfw_extension_count);

    vk::InstanceCreateInfo create_info({}, &app_info, {}, extensions);

    try {
        instance_ = vk::createInstance(create_info);
    } catch (const vk::Error& e) {
        throw std::runtime_error(std::string("Failed to create Vulkan instance: ") + e.what());
    }
}

void VulkanRenderer::createSurface(GLFWwindow* window)
{
    VkSurfaceKHR c_surface;
    if (glfwCreateWindowSurface(static_cast<VkInstance>(instance_), window, nullptr, &c_surface)
        != VK_SUCCESS) {
        throw std::runtime_error("Failed to create Vulkan surface");
    }
    surface_ = c_surface;
}

void VulkanRenderer::selectPhysicalDevice()
{
    auto devices = instance_.enumeratePhysicalDevices();
    if (devices.empty()) {
        throw std::runtime_error("No Vulkan-capable GPU found");
    }

    // Select first GPU (could be improved with scoring)
    physical_device_ = devices[0];

    auto props = physical_device_.getProperties();
    std::cout << "  Using GPU: " << props.deviceName << std::endl;
}

void VulkanRenderer::createLogicalDevice()
{
    // Find graphics queue family
    auto queue_families = physical_device_.getQueueFamilyProperties();
    for (uint32_t i = 0; i < queue_families.size(); ++i) {
        if (queue_families[i].queueFlags & vk::QueueFlagBits::eGraphics) {
            graphics_queue_family_ = i;
            break;
        }
    }

    float queue_priority = 1.0f;
    vk::DeviceQueueCreateInfo queue_create_info({}, graphics_queue_family_, 1, &queue_priority);

    vk::PhysicalDeviceFeatures device_features{};

    std::vector<const char*> device_extensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    vk::DeviceCreateInfo create_info({}, queue_create_info, {}, device_extensions, &device_features);

    device_ = physical_device_.createDevice(create_info);
    graphics_queue_ = device_.getQueue(graphics_queue_family_, 0);
}

void VulkanRenderer::createMemoryAllocator()
{
    VmaAllocatorCreateInfo allocator_info = {};
    allocator_info.physicalDevice = static_cast<VkPhysicalDevice>(physical_device_);
    allocator_info.device = static_cast<VkDevice>(device_);
    allocator_info.instance = static_cast<VkInstance>(instance_);
    allocator_info.vulkanApiVersion = VK_API_VERSION_1_2;

    if (vmaCreateAllocator(&allocator_info, &allocator_) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create VMA allocator");
    }
}

void VulkanRenderer::createSwapchain()
{
    auto surface_capabilities = physical_device_.getSurfaceCapabilitiesKHR(surface_);
    auto surface_formats = physical_device_.getSurfaceFormatsKHR(surface_);
    auto surface_present_modes = physical_device_.getSurfacePresentModesKHR(surface_);

    if (surface_formats.empty() || surface_present_modes.empty()) {
        throw std::runtime_error("No suitable surface formats or present modes available");
    }

    // Select format and present mode
    swapchain_format_ = surface_formats[0].format;
    vk::PresentModeKHR present_mode = vk::PresentModeKHR::eFifo;  // Guaranteed to be available

    // Prefer Mailbox (triple buffering)
    for (auto mode : surface_present_modes) {
        if (mode == vk::PresentModeKHR::eMailbox) {
            present_mode = mode;
            break;
        }
    }

    // Image count (min + 1 or max)
    uint32_t image_count = surface_capabilities.minImageCount + 1;
    if (surface_capabilities.maxImageCount > 0 &&
        image_count > surface_capabilities.maxImageCount) {
        image_count = surface_capabilities.maxImageCount;
    }

    // Extent
    swapchain_extent_ =
        (surface_capabilities.currentExtent.width != UINT32_MAX)
            ? surface_capabilities.currentExtent
            : vk::Extent2D{surface_width_, surface_height_};

    vk::SwapchainCreateInfoKHR create_info({}, surface_, image_count,
        swapchain_format_, surface_formats[0].colorSpace,
        swapchain_extent_, 1,
        vk::ImageUsageFlagBits::eColorAttachment,
        vk::SharingMode::eExclusive, {},
        surface_capabilities.currentTransform,
        vk::CompositeAlphaFlagBitsKHR::eOpaque,
        present_mode, VK_TRUE);

    swapchain_ = device_.createSwapchainKHR(create_info);
    swapchain_images_ = device_.getSwapchainImagesKHR(swapchain_);
}

void VulkanRenderer::createImageViews()
{
    swapchain_image_views_.clear();
    swapchain_image_views_.reserve(swapchain_images_.size());

    for (const auto& image : swapchain_images_) {
        vk::ComponentMapping components(
            vk::ComponentSwizzle::eIdentity,
            vk::ComponentSwizzle::eIdentity,
            vk::ComponentSwizzle::eIdentity,
            vk::ComponentSwizzle::eIdentity);

        vk::ImageSubresourceRange subresource_range(
            vk::ImageAspectFlagBits::eColor,
            0, 1, 0, 1);

        vk::ImageViewCreateInfo create_info(
            {}, image,
            vk::ImageViewType::e2D,
            swapchain_format_,
            components,
            subresource_range);

        swapchain_image_views_.push_back(device_.createImageView(create_info));
    }
}

void VulkanRenderer::createRenderPass()
{
    vk::AttachmentDescription color_attachment(
        {}, swapchain_format_,
        vk::SampleCountFlagBits::e1,
        vk::AttachmentLoadOp::eClear,
        vk::AttachmentStoreOp::eStore,
        vk::AttachmentLoadOp::eDontCare,
        vk::AttachmentStoreOp::eDontCare,
        vk::ImageLayout::eUndefined,
        vk::ImageLayout::ePresentSrcKHR);

    vk::AttachmentReference color_attachment_ref(0, vk::ImageLayout::eColorAttachmentOptimal);

    vk::SubpassDescription subpass(
        {}, vk::PipelineBindPoint::eGraphics,
        0, nullptr,
        1, &color_attachment_ref);

    vk::SubpassDependency dependency(
        VK_SUBPASS_EXTERNAL, 0,
        vk::PipelineStageFlagBits::eColorAttachmentOutput,
        vk::PipelineStageFlagBits::eColorAttachmentOutput,
        {}, vk::AccessFlagBits::eColorAttachmentWrite);

    vk::RenderPassCreateInfo create_info(
        {}, 1, &color_attachment,
        1, &subpass,
        1, &dependency);

    render_pass_ = device_.createRenderPass(create_info);
}

std::vector<uint32_t> readShaderFile(const std::string& filename)
{
    std::ifstream file(filename, std::ios::ate | std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open shader file: " + filename);
    }
    size_t file_size = (size_t)file.tellg();
    std::vector<uint32_t> buffer(file_size / sizeof(uint32_t));
    file.seekg(0);
    file.read((char*)buffer.data(), file_size);
    file.close();
    return buffer;
}

void VulkanRenderer::createDescriptorSetLayout()
{
    // Placeholder - will be populated when we add descriptors
    vk::DescriptorSetLayoutCreateInfo create_info({}, 0, nullptr);
    descriptor_set_layout_ = device_.createDescriptorSetLayout(create_info);
}

void VulkanRenderer::createGraphicsPipeline()
{
    // TODO: Load compiled SPIR-V shaders from binary files
    // For now, this is a stub. In real implementation:
    // auto vert_code = readShaderFile("shaders/particle.vert.spv");
    // auto frag_code = readShaderFile("shaders/particle.frag.spv");

    vk::PipelineLayoutCreateInfo pipeline_layout_info(
        {}, 0, nullptr, 0, nullptr);

    pipeline_layout_ = device_.createPipelineLayout(pipeline_layout_info);

    // Minimal pipeline - will render points
    // Complete implementation requires shader compilation and module loading
    std::cout << "  Note: Graphics pipeline created with minimal setup" << std::endl;
    std::cout << "  Shader binding will be added in next phase" << std::endl;
}

void VulkanRenderer::createFramebuffers()
{
    framebuffers_.clear();
    framebuffers_.reserve(swapchain_image_views_.size());

    for (const auto& image_view : swapchain_image_views_) {
        vk::FramebufferCreateInfo create_info({}, render_pass_, image_view,
            swapchain_extent_.width, swapchain_extent_.height, 1);

        framebuffers_.push_back(device_.createFramebuffer(create_info));
    }
}

void VulkanRenderer::createCommandPool()
{
    vk::CommandPoolCreateInfo create_info(vk::CommandPoolCreateFlagBits::eResetCommandBuffer, graphics_queue_family_);

    command_pool_ = device_.createCommandPool(create_info);
}

void VulkanRenderer::createCommandBuffers()
{
    command_buffers_.clear();
    command_buffers_.resize(swapchain_images_.size());

    vk::CommandBufferAllocateInfo allocate_info(command_pool_, vk::CommandBufferLevel::ePrimary,
        static_cast<uint32_t>(command_buffers_.size()));

    command_buffers_ = device_.allocateCommandBuffers(allocate_info);
}

void VulkanRenderer::createSynchronizationObjects()
{
    uint32_t max_frames_in_flight = 2;

    image_available_semaphores_.resize(max_frames_in_flight);
    render_finished_semaphores_.resize(max_frames_in_flight);
    in_flight_fences_.resize(max_frames_in_flight);

    vk::SemaphoreCreateInfo semaphore_info{};
    vk::FenceCreateInfo fence_info(vk::FenceCreateFlagBits::eSignaled);

    for (uint32_t i = 0; i < max_frames_in_flight; ++i) {
        image_available_semaphores_[i] = device_.createSemaphore(semaphore_info);
        render_finished_semaphores_[i] = device_.createSemaphore(semaphore_info);
        in_flight_fences_[i] = device_.createFence(fence_info);
    }
}

void VulkanRenderer::createParticleBuffer()
{
    // Create a GPU buffer large enough for max 10000 particles
    vk::DeviceSize buffer_size = sizeof(Particle) * 10000;

    vk::BufferCreateInfo buffer_info({}, buffer_size, vk::BufferUsageFlagBits::eStorageBuffer);

    VmaAllocationCreateInfo alloc_info = {};
    alloc_info.usage = VMA_MEMORY_USAGE_AUTO;
    alloc_info.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;

    VkBufferCreateInfo vk_buffer_info = static_cast<VkBufferCreateInfo>(buffer_info);
    VkBuffer vk_particle_buffer = nullptr;

    if (vmaCreateBuffer(allocator_, &vk_buffer_info,
                        &alloc_info, &vk_particle_buffer,
                        &particle_allocation_, nullptr) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create particle buffer");
    }

    particle_buffer_ = vk_particle_buffer;
}

void VulkanRenderer::recreateSwapchain()
{
    device_.waitIdle();
    cleanupSwapchain();

    createSwapchain();
    createImageViews();
    createFramebuffers();
    createCommandBuffers();
}

void VulkanRenderer::cleanupSwapchain()
{
    for (auto& fb : framebuffers_) {
        device_.destroyFramebuffer(fb);
    }
    framebuffers_.clear();

    for (auto& view : swapchain_image_views_) {
        device_.destroyImageView(view);
    }
    swapchain_image_views_.clear();

    device_.destroySwapchainKHR(swapchain_);
}

} // namespace cbps
