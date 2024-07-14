#pragma once

#include <vulkan/vulkan_core.h>
#include <memory>

#include "saltus/vulkan/fwd.hh"

namespace saltus::vulkan
{
    class RawVulkanSemaphore
    {
    public:
        RawVulkanSemaphore(std::shared_ptr<VulkanDevice> device);
        ~RawVulkanSemaphore();

        RawVulkanSemaphore(const RawVulkanSemaphore&) = delete;
        RawVulkanSemaphore(RawVulkanSemaphore&&) = delete;
        RawVulkanSemaphore& operator=(const RawVulkanSemaphore&) = delete;
        RawVulkanSemaphore& operator=(RawVulkanSemaphore&&) = delete;

        const std::shared_ptr<VulkanDevice> &device() const;
        const VkSemaphore &handle() const;

        operator VkSemaphore() const;

    private:
        std::shared_ptr<VulkanDevice> device_;

        VkSemaphore semaphore_ = VK_NULL_HANDLE;
    };
} // namespace saltus::vulkan

