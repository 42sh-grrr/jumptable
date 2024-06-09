#pragma once

#include "saltus/renderer.hh"

#include <optional>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace saltus
{
    struct QueueFamilyIndices
    {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;

        bool is_complete();
    };

    struct SwapChainSupportDetails
    {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> present_modes;
    };

    class VulkanRenderer: public Renderer
    {
    public:
        VulkanRenderer(Window &window);
        ~VulkanRenderer();

        void render() override;
        void wait_for_idle() override;

    private:
        bool validation_enabled_;
        
        VkInstance instance_ = nullptr;
        VkSurfaceKHR surface_ = nullptr;
        VkPhysicalDevice physical_device_ = nullptr;
        VkDevice device_ = nullptr;
        
        VkQueue graphics_queue_ = nullptr;
        VkQueue present_queue_ = nullptr;

        VkFormat swapchain_image_format_;
        VkExtent2D swapchain_extent_;
        VkSwapchainKHR swapchain_;
        std::vector<VkImage> swapchain_images_;
        std::vector<VkImageView> swapchain_image_views_;
        std::vector<VkFramebuffer> swapchain_framebuffers_;

        VkRenderPass render_pass_;
        VkPipelineLayout pipeline_layout_;
        VkPipeline graphics_pipeline_;

        VkCommandPool command_pool_;
        VkCommandBuffer command_buffer_;

        VkSemaphore image_available_semaphore_;
        VkSemaphore render_finished_semaphore_;
        VkFence in_flight_fence_;

        QueueFamilyIndices get_physical_device_family_indices(VkPhysicalDevice device);
        SwapChainSupportDetails get_physical_device_swap_chain_support_details(VkPhysicalDevice device);
        VkShaderModule create_shader_module(const std::vector<char> &code);

        void create_instance();
        void create_surface();
        bool is_physical_device_suitable(VkPhysicalDevice physical_device);
        void choose_physical_device();
        void create_device();
        VkSurfaceFormatKHR choose_swap_chain_format(
            const std::vector<VkSurfaceFormatKHR> &availableFormats
        );
        VkPresentModeKHR choose_swap_chain_present_mode(
            const std::vector<VkPresentModeKHR> &availablePresentModes
        );
        VkExtent2D choose_swap_extent(
            const VkSurfaceCapabilitiesKHR &capabilities
        );
        void create_swap_chain();
        void create_image_views();
        void create_render_pass();
        void create_graphics_pipeline();
        void create_frame_buffers();
        void create_command_pool_and_buffer();
        void create_sync_objects();

        void clean_swap_chain();
        void recreate_swap_chain();

        void record_command_buffer(
            VkCommandBuffer command_buffer, uint32_t image_index
        );
    };
}
