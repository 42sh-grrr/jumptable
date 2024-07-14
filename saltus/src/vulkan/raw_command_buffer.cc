#include "saltus/vulkan/raw_command_buffer.hh"

#include <vulkan/vulkan_core.h>

#include "saltus/vulkan/vulkan_device.hh"
#include "saltus/vulkan/raw_vulkan_fence.hh"

namespace saltus::vulkan
{
    RawCommandBuffer::RawCommandBuffer(std::shared_ptr<VulkanDevice> device, bool resettable):
        device_(device)
    {
        if (resettable)
        {
            command_pool_ = device->resettable_command_buffer_pool();
        }
        else
        {
            command_pool_ = device->transient_command_buffer_pool();
        }

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = command_pool_;
        allocInfo.commandBufferCount = 1;

        vkAllocateCommandBuffers(*device, &allocInfo, &command_buffer_);
    }

    RawCommandBuffer::~RawCommandBuffer()
    {
        if (command_buffer_ != VK_NULL_HANDLE && command_pool_ != VK_NULL_HANDLE)
            vkFreeCommandBuffers(*device_, command_pool_, 1, &command_buffer_);
    }

    const std::shared_ptr<VulkanDevice> &RawCommandBuffer::device() const
    {
        return device_;
    }

    const VkCommandPool &RawCommandBuffer::pool() const
    {
        return command_pool_;
    }

    const VkCommandBuffer &RawCommandBuffer::handle() const
    {
        return command_buffer_;
    }

    void RawCommandBuffer::begin()
    {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(command_buffer_, &beginInfo);
    }

    void RawCommandBuffer::end_and_submit(VkQueue queue, RawVulkanFence *fence)
    {
        vkEndCommandBuffer(command_buffer_);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &command_buffer_;

        vkQueueSubmit(queue, 1, &submitInfo, fence ? fence->handle() : nullptr);
    }
} // namespace saltus::vulkan
