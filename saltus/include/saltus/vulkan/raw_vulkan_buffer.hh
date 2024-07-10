#pragma once

#include <vulkan/vulkan_core.h>
#include "saltus/vulkan/vulkan_device.hh"

namespace saltus::vulkan
{
    class RawVulkanBuffer
    {
    public:
        RawVulkanBuffer(
            std::shared_ptr<VulkanDevice> device,
            VkDeviceSize size,
            VkBufferUsageFlags usage_flags
        );
        ~RawVulkanBuffer();

        VkDeviceSize size() const;

        operator VkBuffer() const;
        VkBuffer buffer_handle() const;

        void alloc(VkMemoryPropertyFlags memory_properties);
        void *map(VkDeviceSize offset, VkDeviceSize size);
        void unmap();

        void copy(
            RawVulkanBuffer &buffer,
            VkDeviceSize src_offset = 0,
            VkDeviceSize dst_offset = 0,
            VkDeviceSize size = VK_WHOLE_SIZE,
            bool wait = true
        );

    private:
        std::shared_ptr<VulkanDevice> device_;

        VkDeviceSize size_;

        VkBuffer buffer_ = VK_NULL_HANDLE;
        VkDeviceMemory memory_ = VK_NULL_HANDLE;
    };
} // namespace saltus::vulkan
