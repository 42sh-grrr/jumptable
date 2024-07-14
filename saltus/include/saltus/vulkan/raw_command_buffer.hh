#pragma once

#include <vulkan/vulkan_core.h>
#include <memory>

#include "saltus/vulkan/fwd.hh"

namespace saltus::vulkan
{
    class RawCommandBuffer
    {
    public:
        RawCommandBuffer(std::shared_ptr<VulkanDevice> device, bool resettable = false);
        ~RawCommandBuffer();

        RawCommandBuffer(const RawCommandBuffer&) = delete;
        RawCommandBuffer(RawCommandBuffer&&) = delete;
        RawCommandBuffer& operator=(const RawCommandBuffer&) = delete;
        RawCommandBuffer& operator=(RawCommandBuffer&&) = delete;

        const std::shared_ptr<VulkanDevice> &device() const;

        const VkCommandPool &pool() const;
        const VkCommandBuffer &handle() const;

        void begin();
        void end_and_submit(VkQueue queue, RawVulkanFence *fence = nullptr);

    private:
        std::shared_ptr<VulkanDevice> device_;

        VkCommandPool command_pool_ = VK_NULL_HANDLE;
        VkCommandBuffer command_buffer_ = VK_NULL_HANDLE;
    };
} // namespace saltus::vulkan
