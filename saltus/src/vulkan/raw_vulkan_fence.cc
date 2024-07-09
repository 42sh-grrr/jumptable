#include "saltus/vulkan/raw_vulkan_fence.hh"

#include <stdexcept>
#include <vulkan/vulkan_core.h>

namespace saltus::vulkan
{
    RawVulkanFence::RawVulkanFence(std::shared_ptr<VulkanDevice> device, bool signaled):
        device_(device)
    {
        VkFenceCreateInfo create_info{};
        create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        if (signaled)
            create_info.flags |= VK_FENCE_CREATE_SIGNALED_BIT;
        
        VkResult result = vkCreateFence(*device, &create_info, nullptr, &fence_);
        if (result != VK_SUCCESS)
            throw std::runtime_error("Could not create fence");
    }

    RawVulkanFence::RawVulkanFence(RawVulkanFence&& other):
        device_(other.device_)
    {
        std::swap(fence_, other.fence_);
    }

    RawVulkanFence::~RawVulkanFence()
    {
        if (device_ && fence_ != VK_NULL_HANDLE)
            vkDestroyFence(*device_, fence_, nullptr);
    }

    const std::shared_ptr<VulkanDevice> &RawVulkanFence::device() const
    {
        return device_;
    }

    const VkFence &RawVulkanFence::handle() const
    {
        return fence_;
    }

    RawVulkanFence::operator VkFence() const
    {
        return fence_;
    }

    bool RawVulkanFence::wait(uint64_t timeout) const
    {
        VkResult result = vkWaitForFences(*device_, 1, &fence_, VK_TRUE, timeout);
        if (result != VK_SUCCESS && result != VK_TIMEOUT)
            throw std::runtime_error("Could not wait for fences");
        return result == VK_TIMEOUT;
    }

    bool RawVulkanFence::get_status() const
    {
        VkResult result = vkGetFenceStatus(*device_, fence_);
        if (result == VK_SUCCESS)
            return true;
        if (result == VK_NOT_READY)
            return false;
        throw std::runtime_error("Could not get fence status");
    }

    void RawVulkanFence::reset()
    {
        VkResult result = vkResetFences(*device_, 1, &fence_);
        if (result != VK_SUCCESS)
            throw std::runtime_error("Could not reset");
    }
} // namespace saltus::vulkan


