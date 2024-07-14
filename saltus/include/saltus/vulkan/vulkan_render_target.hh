#pragma once

#include <memory>
#include <vulkan/vulkan_core.h>

#include "saltus/renderer.hh"
#include "saltus/vulkan/frame_resource.hh"
#include "saltus/vulkan/fwd.hh"

namespace saltus::vulkan
{
    VkPresentModeKHR renderer_present_mode_to_vulkan_present_mode(RendererPresentMode);
    RendererPresentMode vulkan_present_mode_to_renderer_present_mode(VkPresentModeKHR);

    class VulkanRenderTarget: public std::enable_shared_from_this<VulkanRenderTarget>
    {
    public:
        class RenderBuffer
        {
        public:
            RenderBuffer(
                std::shared_ptr<VulkanRenderTarget> render_target,
                uint32_t frame_index,
                bool is_depth
            );
            ~RenderBuffer();

            RenderBuffer(const RenderBuffer&) = delete;
            RenderBuffer(RenderBuffer&&) = delete;
            RenderBuffer& operator=(const RenderBuffer&) = delete;
            RenderBuffer& operator=(RenderBuffer&&) = delete;

            const std::shared_ptr<VulkanRenderTarget> &render_target() const;

            bool is_depth() const;

            const std::shared_ptr<RawVulkanImage> &image() const;
            const std::shared_ptr<RawVulkanImageView> &image_view() const;

        private:
            std::shared_ptr<VulkanRenderTarget> render_target_;

            bool is_depth_;
           
            std::shared_ptr<RawVulkanImage> image_;
            std::shared_ptr<RawVulkanImageView> image_view_;
        };

        VulkanRenderTarget(
            std::shared_ptr<FrameRing> frame_ring,
            std::shared_ptr<VulkanDevice> device,
            RendererPresentMode target_present_mode,
            MsaaSamples msaa_samples
        );
        ~VulkanRenderTarget();

        const std::shared_ptr<FrameRing> &frame_ring() const;
        const std::shared_ptr<VulkanDevice> &device() const;

        const RendererPresentMode &target_present_mode() const;
        void target_present_mode(RendererPresentMode);

        VkSampleCountFlagBits msaa_sample_bits() const;

        const VkFormat &swapchain_image_format() const;
        const VkExtent2D &swapchain_extent() const;
        const VkSwapchainKHR &swapchain() const;
        const VkPresentModeKHR &present_mode() const;
        const std::vector<VkImage> &swapchain_images() const;
        const std::vector<VkImageView> &swapchain_image_views() const;

        const VkFormat &depth_format() const;
        const FrameResource<RenderBuffer> &depth_resource() const;
        
        void resize_if_changed();
        /// Full recreate the swapchain, necessary when for example a resize
        /// occures
        void recreate();

        uint32_t acquire_next_image(VkSemaphore semaphore, VkFence fence = VK_NULL_HANDLE);

        VkImage get_render_image(uint32_t acquired_image, uint32_t frame_index);
        VkImageView get_render_image_view(uint32_t acquired_image, uint32_t frame_index);
        VkImage get_present_image(uint32_t acquired_image, uint32_t frame_index);
        VkImageView get_present_image_view(uint32_t acquired_image, uint32_t frame_index);

    private:
        std::shared_ptr<FrameRing> frame_ring_;
        std::shared_ptr<VulkanDevice> device_;

        uint32_t msaa_samples_;
        RendererPresentMode target_present_mode_;

        VkFormat swapchain_image_format_;
        VkExtent2D swapchain_extent_;
        VkSwapchainKHR swapchain_;
        VkPresentModeKHR present_mode_;
        std::vector<VkImage> swapchain_images_;
        std::vector<VkImageView> swapchain_image_views_;

        VkFormat depth_format_;
        FrameResource<RenderBuffer> depth_resource_;
        std::optional<FrameResource<RenderBuffer>> backbuffer_resource_;

        VkSurfaceFormatKHR choose_swap_chain_format(
            const std::vector<VkSurfaceFormatKHR> &availableFormats
        );
        VkPresentModeKHR choose_swap_chain_present_mode(
            const std::vector<VkPresentModeKHR> &availablePresentModes
        );
        VkExtent2D choose_swap_extent(
            const VkSurfaceCapabilitiesKHR &capabilities
        );

        void create();

        void create_swap_chain();
        void create_images();
        void create_image_views();

        void destroy();

        void destroy_swap_chain();
        void destroy_image_views();
    };
} // namespace saltus::vulkan
