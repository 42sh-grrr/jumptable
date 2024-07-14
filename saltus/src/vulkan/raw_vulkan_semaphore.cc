#include "saltus/vulkan/raw_vulkan_semaphore.hh"

#include <stdexcept>
#include <vulkan/vulkan_core.h>

#include "saltus/vulkan/vulkan_device.hh"

namespace saltus::vulkan
{
    RawVulkanSemaphore::RawVulkanSemaphore(std::shared_ptr<VulkanDevice> device):
        device_(device)
    {
        VkSemaphoreCreateInfo create_info{};

        
        VkResult result = vkCreateSemaphore(*device, &create_info, nullptr, &semaphore_);
        if (result != VK_SUCCESS)
            throw std::runtime_error("Could not create semaphore");
    }

    RawVulkanSemaphore::~RawVulkanSemaphore()
    {
        if (device_ && semaphore_ != VK_NULL_HANDLE)
            vkDestroySemaphore(*device_, semaphore_, nullptr);
    }

    const std::shared_ptr<VulkanDevice> &RawVulkanSemaphore::device() const
    {
        return device_;
    }

    const VkSemaphore &RawVulkanSemaphore::handle() const
    {
        return semaphore_;
    }

    RawVulkanSemaphore::operator VkSemaphore() const
    {
        return semaphore_;
    }
} // namespace saltus::vulkan

