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

        QueueFamilyIndices get_physical_device_family_indices(VkPhysicalDevice device);
        SwapChainSupportDetails get_physical_device_swap_chain_support_details(VkPhysicalDevice device);

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
    };
}
