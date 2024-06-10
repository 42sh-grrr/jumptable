#pragma once

#include <optional>
#include <vector>
#include <vulkan/vulkan_core.h>

#include "saltus/vulkan/vulkan_instance.hh"
#include "saltus/window.hh"

namespace saltus::vulkan
{
    struct SwapChainSupportDetails
    {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> present_modes;
    };

    struct QueueFamilyIndices
    {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;

        bool is_complete();
    };

    class VulkanDevice
    {
    public:
        VulkanDevice(const Window &window, std::shared_ptr<VulkanInstance> instance);
        ~VulkanDevice();

        operator VkDevice() const;

        QueueFamilyIndices get_physical_device_family_indices() const;
        QueueFamilyIndices get_physical_device_family_indices(VkPhysicalDevice device) const;
        SwapChainSupportDetails get_physical_device_swap_chain_support_details() const;
        SwapChainSupportDetails get_physical_device_swap_chain_support_details(
            VkPhysicalDevice physical_device
        ) const;

        VkSurfaceKHR surface() const;
        VkPhysicalDevice physical_device() const;
        VkDevice device() const;

        VkQueue graphics_queue();
        VkQueue present_queue();

    private:
        std::shared_ptr<VulkanInstance> instance_;

        VkSurfaceKHR surface_ = nullptr;
        VkPhysicalDevice physical_device_ = nullptr;
        VkDevice device_ = nullptr;

        VkQueue graphics_queue_ = nullptr;
        VkQueue present_queue_ = nullptr;

        bool is_physical_device_suitable(VkPhysicalDevice physical_device);
        void choose_physical_device();
    };
} // namespace saltus::vulkan
