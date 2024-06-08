#pragma once

#include "saltus/renderer.hh"

#include <optional>
#include <vulkan/vulkan_core.h>

namespace saltus
{
    struct QueueFamilyIndices
    {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;

        bool is_complete();
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

        QueueFamilyIndices get_physical_device_family_indices(VkPhysicalDevice device);

        void create_instance();
        void create_surface();
        bool is_physical_device_suitable(VkPhysicalDevice physical_device);
        void choose_physical_device();
        void create_device();
    };
}
