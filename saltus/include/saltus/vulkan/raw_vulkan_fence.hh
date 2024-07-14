#pragma once

#include <vulkan/vulkan_core.h>
#include <memory>

#include "saltus/vulkan/fwd.hh"

namespace saltus::vulkan
{
    class RawVulkanFence
    {
    public:
        RawVulkanFence(std::shared_ptr<VulkanDevice> device, bool signaled = false);
        RawVulkanFence(RawVulkanFence&&);
        ~RawVulkanFence();

        RawVulkanFence(const RawVulkanFence&) = delete;
        RawVulkanFence& operator=(const RawVulkanFence&) = delete;
        RawVulkanFence& operator=(RawVulkanFence&&) = delete;

        const std::shared_ptr<VulkanDevice> &device() const;
        const VkFence &handle() const;

        operator VkFence() const;

        /// Returns true if timedout
        bool wait(uint64_t timeout = ~0) const;

        bool get_status() const;
        void reset();

    private:
        std::shared_ptr<VulkanDevice> device_;

        VkFence fence_ = VK_NULL_HANDLE;
    };
} // namespace saltus::vulkan


