#pragma once

#include "saltus/renderer.hh"

#include <vulkan/vulkan_core.h>

namespace saltus
{
    class VulkanRenderer: public Renderer
    {
    public:
        VulkanRenderer();
        ~VulkanRenderer();

    private:
        void create_instance();
        bool is_physical_device_suitable(VkPhysicalDevice physical_device);
        void choose_physical_device();
        void create_device();

        bool validation_enabled_;
        
        VkInstance instance_ = nullptr;
        VkPhysicalDevice physical_device_ = nullptr;
        VkDevice device_ = nullptr;

        VkQueue graphics_queue_ = nullptr;
    };
}
