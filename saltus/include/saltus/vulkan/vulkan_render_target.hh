#pragma once

#include <vulkan/vulkan_core.h>
#include "saltus/vulkan/vulkan_device.hh"

namespace saltus::vulkan
{
    class VulkanRenderTarget
    {
    public:
        VulkanRenderTarget(std::shared_ptr<VulkanDevice> device);
        ~VulkanRenderTarget();

        const std::shared_ptr<VulkanDevice> &device() const;

        const VkFormat &swapchain_image_format() const;
        const VkExtent2D &swapchain_extent() const;
        const VkSwapchainKHR &swapchain() const;
        const std::vector<VkImage> &swapchain_images() const;
        const std::vector<VkImageView> &swapchain_image_views() const;
        
        /// Full recreate the swapchain, necessary when for example a resize
        /// occures
        void recreate();

        uint32_t acquire_next_image(VkSemaphore semaphore, VkFence fence = VK_NULL_HANDLE);

    private:
        std::shared_ptr<VulkanDevice> device_;

        VkFormat swapchain_image_format_;
        VkExtent2D swapchain_extent_;
        VkSwapchainKHR swapchain_;
        std::vector<VkImage> swapchain_images_;
        std::vector<VkImageView> swapchain_image_views_;

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
