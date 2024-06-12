#pragma once

#include "saltus/renderer.hh"
#include "saltus/vulkan/vulkan_device.hh"
#include "saltus/vulkan/vulkan_instance.hh"

#include <vector>
#include <vulkan/vulkan_core.h>

namespace saltus::vulkan
{
    class VulkanRenderer: public Renderer
    {
    public:
        VulkanRenderer(Window &window);
        ~VulkanRenderer();

        void render() override;
        void wait_for_idle() override;

        std::shared_ptr<Shader> create_shader(ShaderCreateInfo info) override;
        std::shared_ptr<Material> create_material(MaterialCreateInfo) override;
        std::shared_ptr<Mesh> create_mesh(MeshCreateInfo) override;

    private:
        std::shared_ptr<VulkanInstance> instance_;
        std::shared_ptr<VulkanDevice> device_;

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
        std::vector<VkCommandBuffer> command_buffers_;

        std::vector<VkSemaphore> image_available_semaphores_;
        std::vector<VkSemaphore> render_finished_semaphores_;
        std::vector<VkFence> in_flight_fences_;

        int current_frame_ = 0;

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
        void create_command_pool_and_buffers();
        void create_sync_objects();

        void clean_swap_chain();
        void recreate_swap_chain();

        void record_command_buffer(
            VkCommandBuffer command_buffer, uint32_t image_index
        );
    };
}
