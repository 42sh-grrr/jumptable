#pragma once

#include <vulkan/vulkan_core.h>
namespace saltus::vulkan
{
    class VulkanInstance
    {
    public:
        VulkanInstance();
        VulkanInstance(bool try_enable_validation);
        ~VulkanInstance();

        VulkanInstance(const VulkanInstance &) = delete;
        VulkanInstance &operator =(const VulkanInstance &) = delete;

        operator VkInstance() const;

        bool validation_enabled();
        VkInstance instance();

    private:
        bool validation_enabled_;
        VkInstance instance_;
    };
}
